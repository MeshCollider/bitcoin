// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/rpcutil.h>

#include <random.h>

#include <fs.h>

/** Username used when cookie authentication is in use (arbitrary, only for
 * recognizability in debugging/logging purposes)
 */
static const std::string COOKIEAUTH_USER = "__cookie__";
/** Default name for auth cookie file */
static const std::string COOKIEAUTH_FILE = ".cookie";

/** Get name of RPC authentication cookie file */
static fs::path GetAuthCookieFile(bool temp=false)
{
    std::string arg = g_rpc_args.cookie_file;
    if (temp) {
        arg += ".tmp";
    }
    fs::path path(arg);
    if (!path.is_complete()) path = GetDataDir() / path;
    return path;
}

bool GenerateAuthCookie(std::string *cookie_out)
{
    const size_t COOKIE_SIZE = 32;
    unsigned char rand_pwd[COOKIE_SIZE];
    GetRandBytes(rand_pwd, COOKIE_SIZE);
    std::string cookie = COOKIEAUTH_USER + ":" + HexStr(rand_pwd, rand_pwd+COOKIE_SIZE);

    /** the umask determines what permissions are used to create this file -
     * these are set to 077 in init.cpp unless overridden with -sysperms.
     */
    std::ofstream file;
    fs::path filepath_tmp = GetAuthCookieFile(true);
    file.open(filepath_tmp.string().c_str());
    if (!file.is_open()) {
        LogPrintf("Unable to open cookie authentication file %s for writing\n", filepath_tmp.string());
        return false;
    }
    file << cookie;
    file.close();

    fs::path filepath = GetAuthCookieFile(false);
    if (!RenameOver(filepath_tmp, filepath)) {
        LogPrintf("Unable to rename cookie authentication file %s to %s\n", filepath_tmp.string(), filepath.string());
        return false;
    }
    LogPrintf("Generated RPC authentication cookie %s\n", filepath.string());

    if (cookie_out)
        *cookie_out = cookie;
    return true;
}

bool GetAuthCookie(std::string *cookie_out)
{
    std::ifstream file;
    std::string cookie;
    fs::path filepath = GetAuthCookieFile();
    file.open(filepath.string().c_str());
    if (!file.is_open())
        return false;
    std::getline(file, cookie);
    file.close();

    if (cookie_out)
        *cookie_out = cookie;
    return true;
}

void DeleteAuthCookie()
{
    try {
        fs::remove(GetAuthCookieFile());
    } catch (const fs::filesystem_error& e) {
        LogPrintf("%s: Unable to remove random auth cookie file: %s\n", __func__, e.what());
    }
}

RPCArguments g_rpc_args;

static const ArgumentEntry rpcArgs[] =
{ //  name              type             global variable             default value
  //  ----------------- ---------------- --------------------------- ----------
    {"-rpcauth",        ARG_STRING_VEC,  &g_rpc_args.rpcauth,        ""},
    {"-rpcuser",        ARG_STRING,      &g_rpc_args.rpc_user,       ""},
    {"-rpcpassword",    ARG_STRING,      &g_rpc_args.rpc_password,   ""},
    {"-rpcallowip",     ARG_STRING_VEC,  &g_rpc_args.rpc_allow_ip,   ""},
    {"-rpcport",        ARG_INT,         &g_rpc_args.rpc_port,       ""},
    {"-rpcbind",        ARG_STRING_VEC,  &g_rpc_args.rpc_bind,       ""},
    {"-rpccookiefile",  ARG_STRING,      &g_rpc_args.cookie_file,    COOKIEAUTH_FILE},
    {"-rpcssl",         ARG_BOOL,        &g_rpc_args.rpcssl,         "0"},
    {"-rpcservertimeout", ARG_INT,       &g_rpc_args.rpc_server_timeout, std::to_string(DEFAULT_HTTP_SERVER_TIMEOUT)},
    {"-rpcworkqueue",   ARG_INT,         &g_rpc_args.rpc_work_queue,  std::to_string(DEFAULT_HTTP_WORKQUEUE)},
    {"-rpcthreads",     ARG_INT,         &g_rpc_args.rpc_threads,     std::to_string(DEFAULT_HTTP_THREADS)},
};

void RegisterRPCArguments() {
    for (unsigned int i = 0; i < ARRAYLEN(rpcArgs); i++) {
        gArgs.RegisterArg(rpcArgs[i].name, &rpcArgs[i]);
    }
}
