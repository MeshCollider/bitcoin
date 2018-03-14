// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <args.h>

#include <util.h>

#include <boost/algorithm/string/case_conv.hpp> // for to_lower()
#include <boost/algorithm/string/predicate.hpp> // for startswith() and endswith()
#include <boost/program_options/detail/config_file.hpp>

ArgsManager gArgs;

/** Interpret string as boolean, for argument parsing */
static bool InterpretBool(const std::string& strValue)
{
    if (strValue.empty())
        return true;
    return (atoi(strValue) != 0);
}

/** Turn -noX into -X=0 */
static void InterpretNegativeSetting(std::string& strKey, std::string& strValue)
{
    if (strKey.length()>3 && strKey[0]=='-' && strKey[1]=='n' && strKey[2]=='o')
    {
        strKey = "-" + strKey.substr(3);
        strValue = InterpretBool(strValue) ? "0" : "1";
    }
}

void ArgsManager::ParseParameters(int argc, const char* const argv[], bool ignore_extra)
{
    LOCK(cs_args);
    mapArgs.clear();
    mapMultiArgs.clear();

    for (int i = 1; i < argc; i++)
    {
        std::string str(argv[i]);
        std::string strValue;
        size_t is_index = str.find('=');
        if (is_index != std::string::npos)
        {
            strValue = str.substr(is_index+1);
            str = str.substr(0, is_index);
        }
#ifdef WIN32
        boost::to_lower(str);
        if (boost::algorithm::starts_with(str, "/"))
            str = "-" + str.substr(1);
#endif

        if (str[0] != '-')
            break;

        // Interpret --foo as -foo.
        // If both --foo and -foo are set, the last takes effect.
        if (str.length() > 1 && str[1] == '-')
            str = str.substr(1);
        InterpretNegativeSetting(str, strValue);

        mapArgs[str] = strValue;
        mapMultiArgs[str].push_back(strValue);
        is_arg_set[str] = true;

        SetArg(str, strValue, ignore_extra);
    }
}

std::vector<std::string> ArgsManager::GetArgs(const std::string& strArg) const
{
    LOCK(cs_args);
    auto it = mapMultiArgs.find(strArg);
    if (it != mapMultiArgs.end()) return it->second;
    return {};
}

bool ArgsManager::IsArgSet(const std::string& strArg) const
{
    LOCK(cs_args);
    auto it = is_arg_set.find(strArg);
    if (it != is_arg_set.end()) return it->second;
    return false;
}

std::string ArgsManager::GetArg(const std::string& strArg, const std::string& strDefault) const
{
    LOCK(cs_args);
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return it->second;
    return strDefault;
}

int64_t ArgsManager::GetArg(const std::string& strArg, int64_t nDefault) const
{
    LOCK(cs_args);
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return atoi64(it->second);
    return nDefault;
}

bool ArgsManager::GetBoolArg(const std::string& strArg, bool fDefault) const
{
    LOCK(cs_args);
    auto it = mapArgs.find(strArg);
    if (it != mapArgs.end()) return InterpretBool(it->second);
    return fDefault;
}

bool ArgsManager::SoftSetArg(const std::string& strArg, const std::string& strValue)
{
    LOCK(cs_args);
    if (IsArgSet(strArg)) return false;
    ForceSetArg(strArg, strValue);
    return true;
}

bool ArgsManager::SoftSetBoolArg(const std::string& strArg, bool fValue)
{
    if (fValue)
        return SoftSetArg(strArg, std::string("1"));
    else
        return SoftSetArg(strArg, std::string("0"));
}

void ArgsManager::ForceSetArg(const std::string& strArg, const std::string& strValue)
{
    LOCK(cs_args);
    mapArgs[strArg] = strValue;
    mapMultiArgs[strArg] = {strValue};
    SetArg(strArg, strValue, false, true);
    is_arg_set[strArg] = true;
}

void ArgsManager::SetArg(const std::string& arg_name, const std::string& arg_value, bool ignore_extra, bool force_set) {
    LOCK(cs_args);

    auto it = arguments.find(arg_name);
    if (it != arguments.end()) {
        const ArgumentEntry* arg = it->second;

        // if this arg has already been set, only set it again if it can take multiple values
        if (!force_set && is_arg_set[arg_name] && arg->arg_type != ARG_STRING_VEC) return;

        if (arg->arg_type == ARG_BOOL) {
            bool value = InterpretBool(arg_value);
            *static_cast<bool*>(arg->destination_var) = value;
        } else if (arg->arg_type == ARG_INT) {
            int value = atoi(arg_value);
            *static_cast<int*>(arg->destination_var) = value;
        } else if (arg->arg_type == ARG_STRING) {
            *static_cast<std::string*>(arg->destination_var) = arg_value;
        } else if (arg->arg_type == ARG_STRING_VEC) {
            std::vector<std::string>* temp = static_cast<std::vector<std::string>*>(arg->destination_var);
            if (!arg_value.empty() || (force_set && temp->empty())) {
                temp->push_back(arg_value);
            }
        }
    } else if (!ignore_extra) {
        // print error because this argument is unrecognised
        // TODO: uncomment this when all args are converted:
        //throw std::runtime_error(strprintf("unrecognised argument \"%s\". Please use -? or -help for more information.", str));
    }
}

void ArgsManager::ReadConfigFile(const std::string& confPath)
{
    fs::ifstream streamConfig(GetConfigFile(confPath));
    if (!streamConfig.good())
        return; // No bitcoin.conf file is OK

    {
        LOCK(cs_args);
        std::set<std::string> setOptions;
        setOptions.insert("*");

        for (boost::program_options::detail::config_file_iterator it(streamConfig, setOptions), end; it != end; ++it)
        {
            // Don't overwrite existing settings so command line settings override bitcoin.conf
            std::string strKey = std::string("-") + it->string_key;
            std::string strValue = it->value[0];
            InterpretNegativeSetting(strKey, strValue);
            if (!is_arg_set[strKey]) mapArgs[strKey] = strValue;
            SetArg(strKey, strValue, false, false);
            mapMultiArgs[strKey].push_back(strValue);
            is_arg_set[strKey] = true;
        }
    }
    // If datadir is changed in .conf file:
    ClearDatadirCache();
    if (!fs::is_directory(GetDataDir(false))) {
        throw std::runtime_error(strprintf("specified data directory \"%s\" does not exist.", g_file_args.datadir.c_str()));
    }
}

void ArgsManager::RegisterArg(const std::string& name, const ArgumentEntry* arg_entry)
{
    LOCK(cs_args);
    arguments[name] = arg_entry;
    is_arg_set[name] = false;
    SetArg(name, arg_entry->default_value);
}
