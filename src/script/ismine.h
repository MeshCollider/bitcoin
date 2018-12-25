// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCRIPT_ISMINE_H
#define BITCOIN_SCRIPT_ISMINE_H

#include <script/standard.h>

#include <stdint.h>

class CScript;

/** IsMine() return codes */
enum isminetype
{
    ISMINE_NO = 0,
    ISMINE_WATCH_ONLY = 1,
    ISMINE_SPENDABLE = 2,
    ISMINE_ALL = ISMINE_WATCH_ONLY | ISMINE_SPENDABLE
};
/** used for bitflags of isminetype */
typedef uint8_t isminefilter;

/** An interface for different types of IsMine logic. */
class IsMineProvider
{
public:
    virtual ~IsMineProvider() {}
    virtual isminetype IsMine(const CScript& scriptPubKey) const { return false; }
    virtual isminetype IsMine(const CTxDestination& dest) const { return false; }
};

#endif // BITCOIN_SCRIPT_ISMINE_H
