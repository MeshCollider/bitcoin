// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ARGS_H
#define BITCOIN_ARGS_H

#include <sync.h>

#include <utilstrencodings.h>

#include <map>
#include <set>
#include <string>
#include <vector>

// Type of argument value, for validation
enum ArgValueType {
    ARG_BOOL = 0,
    ARG_INT = 1,
    ARG_STRING = 2,
    ARG_STRING_VEC = 3,
};

class ArgumentEntry
{
public:
    std::string name;
    ArgValueType arg_type;
    void* destination_var;
    std::string default_value;
    /* TODO:
     * documentation strings
     * is_debug
     * category
     */
};

class ArgsManager
{
protected:
    mutable CCriticalSection cs_args;
    std::map<std::string, std::string> mapArgs;
    std::map<std::string, std::vector<std::string>> mapMultiArgs;
    std::map<std::string, const ArgumentEntry*> arguments;
public:
    void ParseParameters(int argc, const char*const argv[], bool ignore_extra=false);
    void ReadConfigFile(const std::string& confPath);

    /**
     * Return a vector of strings of the given argument
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @return command-line arguments
     */
    std::vector<std::string> GetArgs(const std::string& strArg) const;

    /**
     * Return true if the given argument has been manually set
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @return true if the argument has been set
     */
    bool IsArgSet(const std::string& strArg) const;

    /**
     * Return string argument or default value
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param strDefault (e.g. "1")
     * @return command-line argument or default value
     */
    std::string GetArg(const std::string& strArg, const std::string& strDefault) const;

    /**
     * Return integer argument or default value
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param nDefault (e.g. 1)
     * @return command-line argument (0 if invalid number) or default value
     */
    int64_t GetArg(const std::string& strArg, int64_t nDefault) const;

    /**
     * Return boolean argument or default value
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param fDefault (true or false)
     * @return command-line argument or default value
     */
    bool GetBoolArg(const std::string& strArg, bool fDefault) const;

    /**
     * Set an argument if it doesn't already have a value
     *
     * @param strArg Argument to set (e.g. "-foo")
     * @param strValue Value (e.g. "1")
     * @return true if argument gets set, false if it already had a value
     */
    bool SoftSetArg(const std::string& strArg, const std::string& strValue);

    /**
     * Set a boolean argument if it doesn't already have a value
     *
     * @param strArg Argument to set (e.g. "-foo")
     * @param fValue Value (e.g. false)
     * @return true if argument gets set, false if it already had a value
     */
    bool SoftSetBoolArg(const std::string& strArg, bool fValue);

    // Forces an arg setting. Called by SoftSetArg() if the arg hasn't already
    // been set. Also called directly in testing.
    void ForceSetArg(const std::string& strArg, const std::string& strValue);

    // Sets arg using reference table and in mapArgs but does not touch mapMultiArgs
    void SetArg(const std::string& arg_name, const std::string& arg_value, bool ignore_extra=false);

    // Provide the ArgsManager a reference to a global variable to be set
    // to the value of the corresponding command-line argument
    void RegisterArg(const std::string& name, const ArgumentEntry* arg_entry);
};

extern ArgsManager gArgs;

void RegisterMetaArguments();
void RegisterFileArguments();
void RegisterChainArguments();

static inline void RegisterAllArgs()
{
    RegisterMetaArguments();
    RegisterFileArguments();
    RegisterChainArguments();
}

#endif // BITCOIN_ARGS_H
