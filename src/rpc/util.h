// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPC_UTIL_H
#define BITCOIN_RPC_UTIL_H

#include <args.h>
#include <pubkey.h>
#include <script/standard.h>
#include <univalue.h>
#include <utilstrencodings.h>
#include <util.h>

#include <boost/variant/static_visitor.hpp>

#include <string>
#include <vector>

class CKeyStore;
class CPubKey;
class CScript;

CPubKey HexToPubKey(const std::string& hex_in);
CPubKey AddrToPubKey(CKeyStore* const keystore, const std::string& addr_in);
CScript CreateMultisigRedeemscript(const int required, const std::vector<CPubKey>& pubkeys);

UniValue DescribeAddress(const CTxDestination& dest);

static const int DEFAULT_HTTP_THREADS=4;
static const int DEFAULT_HTTP_WORKQUEUE=16;
static const int DEFAULT_HTTP_SERVER_TIMEOUT=30;

extern struct RPCArguments {
    std::vector<std::string> rpcauth;
    std::string rpc_password;
    std::string rpc_user;
    std::vector<std::string> rpc_allow_ip;
    int rpc_port;
    std::vector<std::string> rpc_bind;
    bool rpcssl;
    int rpc_server_timeout;
    int rpc_work_queue;
    int rpc_threads;
} g_rpc_args;

#endif // BITCOIN_RPC_UTIL_H
