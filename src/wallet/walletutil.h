// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_UTIL_H
#define BITCOIN_WALLET_UTIL_H

#include <args.h>
#include <chainparamsbase.h>
#include <util.h>

#include <vector>

//! Get the path of the wallet directory.
fs::path GetWalletDir();

extern struct WalletArguments {
    std::vector<std::string> wallets;
    std::string walletdir;
    int zapwallettxes;
} g_wallet_args;

#endif // BITCOIN_WALLET_UTIL_H
