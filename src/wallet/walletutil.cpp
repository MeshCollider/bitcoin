// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/walletutil.h>

fs::path GetWalletDir()
{
    fs::path path;

    if (g_wallet_args.walletdir != "") {
        path = g_wallet_args.walletdir;
        if (!fs::is_directory(path)) {
            // If the path specified doesn't exist, we return the deliberately
            // invalid empty string.
            path = "";
        }
    } else {
        path = GetDataDir();
        // If a wallets directory exists, use that, otherwise default to GetDataDir
        if (fs::is_directory(path / "wallets")) {
            path /= "wallets";
        }
    }

    return path;
}


WalletArguments g_wallet_args;

static const ArgumentEntry walletArgs[] =
{ //  name            type          global variable         default value
  //  --------------  ------------- ----------------------- ----------
    {"-wallet",       ARG_STRING_VEC,   &g_wallet_args.wallets,     ""},
    {"-walletdir",    ARG_STRING,       &g_wallet_args.walletdir,   ""},
};

void RegisterWalletArguments() {
    for (unsigned int i = 0; i < ARRAYLEN(walletArgs); i++) {
        gArgs.RegisterArg(walletArgs[i].name, &walletArgs[i]);
    }
}
