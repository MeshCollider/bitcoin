// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparamsbase.h>

#include <tinyformat.h>
#include <util.h>

#include <assert.h>

const std::string CBaseChainParams::MAIN = "main";
const std::string CBaseChainParams::TESTNET = "test";
const std::string CBaseChainParams::REGTEST = "regtest";

void AppendParamsHelpMessages(std::string& strUsage, bool debugHelp)
{
    strUsage += HelpMessageGroup(_("Chain selection options:"));
    if (debugHelp) {
        strUsage += HelpMessageOpt("-regtest", "Enter regression test mode, which uses a special chain in which blocks can be solved instantly. "
                                   "This is intended for regression testing tools and app development.");
    }
    strUsage += HelpMessageOpt("-testnet", _("Use the test chain"));
}

static std::unique_ptr<CBaseChainParams> globalChainBaseParams;

const CBaseChainParams& BaseParams()
{
    assert(globalChainBaseParams);
    return *globalChainBaseParams;
}

std::unique_ptr<CBaseChainParams> CreateBaseChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return MakeUnique<CBaseChainParams>("", 8332);
    else if (chain == CBaseChainParams::TESTNET)
        return MakeUnique<CBaseChainParams>("testnet3", 18332);
    else if (chain == CBaseChainParams::REGTEST)
        return MakeUnique<CBaseChainParams>("regtest", 18443);
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectBaseParams(const std::string& chain)
{
    globalChainBaseParams = CreateBaseChainParams(chain);
}

std::string ChainNameFromCommandLine()
{
    if (g_chain_args.testnet && g_chain_args.regtest)
        throw std::runtime_error("Invalid combination of -regtest and -testnet.");
    if (g_chain_args.regtest)
        return CBaseChainParams::REGTEST;
    if (g_chain_args.testnet)
        return CBaseChainParams::TESTNET;
    return CBaseChainParams::MAIN;
}

ChainArguments g_chain_args;

static const ArgumentEntry chainArgs[] =
{ //  name            type       global variable          default value
  //  --------------  ---------- ------------------------ --------------
    {"-regtest",      ARG_BOOL,  &g_chain_args.regtest,   "0"},
    {"-testnet",      ARG_BOOL,  &g_chain_args.testnet,   "0"},
};

void RegisterChainArguments() {
    for (unsigned int i = 0; i < ARRAYLEN(chainArgs); i++) {
        gArgs.RegisterArg(chainArgs[i].name, &chainArgs[i]);
    }
}
