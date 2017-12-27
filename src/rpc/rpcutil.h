// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPCUTIL_H
#define BITCOIN_RPCUTIL_H

#include <args.h>
#include <util.h>

/** Generate a new RPC authentication cookie and write it to disk */
bool GenerateAuthCookie(std::string *cookie_out);
/** Read the RPC authentication cookie from disk */
bool GetAuthCookie(std::string *cookie_out);
/** Delete RPC authentication cookie from disk */
void DeleteAuthCookie();

static const int DEFAULT_HTTP_THREADS=4;
static const int DEFAULT_HTTP_WORKQUEUE=16;
static const int DEFAULT_HTTP_SERVER_TIMEOUT=30;

extern struct RPCArguments {
    std::vector<std::string> rpcauth;
    std::string rpc_password;
    std::string rpc_user;
    std::string cookie_file;
    std::vector<std::string> rpc_allow_ip;
    int rpc_port;
    std::vector<std::string> rpc_bind;
    bool rpcssl;
    int rpc_server_timeout;
    int rpc_work_queue;
    int rpc_threads;
} g_rpc_args;

#endif // BITCOIN_RPCUTIL_H
