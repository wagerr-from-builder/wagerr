// Copyright (c) 2017-2018 The PIVX developers
// Copyright (c) 2018-2020 The Ion developers
// Copyright (c) 2022 The Wagerr developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "arith_uint256.h"
#include "chain.h"
#include "kernel.h"
#include "stakeinput.h"
#include "validation.h"
#include "wallet/wallet.h"
#include "zwgr/accumulators.h"
#include "zwgr/deterministicmint.h"

typedef std::vector<unsigned char> valtype;

CZStake::CZStake(const libzerocoin::CoinSpend& spend)
{
    this->nChecksum = spend.getAccumulatorChecksum();
    this->denom = spend.getDenomination();
    uint256 nSerial = ArithToUint256(spend.getCoinSerialNumber().getuint256());
    this->hashSerial = Hash(nSerial.begin(), nSerial.end());
    fMint = false;
}

int CZStake::GetChecksumHeightFromMint()
{
    int nHeightChecksum = ::ChainActive().Height() - Params().GetConsensus().nZerocoinRequiredStakeDepth;

    //Need to return the first occurance of this checksum in order for the validation process to identify a specific
    //block height
    uint32_t nChecksum = 0;
    nChecksum = ParseChecksum(::ChainActive()[nHeightChecksum]->GetBlockHeader().nAccumulatorCheckpoint, denom);
    return GetChecksumHeight(nChecksum, denom);
}

int CZStake::GetChecksumHeightFromSpend()
{
    return GetChecksumHeight(nChecksum, denom);
}

uint32_t CZStake::GetChecksum()
{
    return nChecksum;
}

// The zWAGERR block index is the first appearance of the accumulator checksum that was used in the spend
// note that this also means when staking that this checksum should be from a block that is beyond 60 minutes old and
// 100 blocks deep.
CBlockIndex* CZStake::GetIndexFrom()
{
    if (pindexFrom)
        return pindexFrom;

    int nHeightChecksum = 0;

    if (fMint)
        nHeightChecksum = GetChecksumHeightFromMint();
    else
        nHeightChecksum = GetChecksumHeightFromSpend();

    if (nHeightChecksum < Params().GetConsensus().nZerocoinStartHeight || nHeightChecksum > ::ChainActive().Height()) {
        pindexFrom = nullptr;
    } else {
        //note that this will be a nullptr if the height DNE
        pindexFrom = ::ChainActive()[nHeightChecksum];
    }

    return pindexFrom;
}

CAmount CZStake::GetValue() const
{
    return denom * COIN;
}

//Use the first accumulator checkpoint that occurs 60 minutes after the block being staked from
// In case of regtest, next accumulator of 60 blocks after the block being staked from
bool CZStake::GetModifier(uint64_t& nStakeModifier)
{
    CBlockIndex* pindex = GetIndexFrom();
    if (!pindex)
        return error("%s: failed to get index from", __func__);

    if(Params().NetworkIDString() == CBaseChainParams::REGTEST) {
        // Stake modifier is fixed for now, move it to 60 blocks after this pindex in the future..
        nStakeModifier = pindexFrom->nStakeModifier;
        return true;
    }

    int64_t nTimeBlockFrom = pindex->GetBlockTime();
    while (true) {
        if (pindex->GetBlockTime() - nTimeBlockFrom > 60 * 60) {
            nStakeModifier = UintToArith256(pindex->GetBlockHeader().nAccumulatorCheckpoint).GetLow64();
            return true;
        }

        if (pindex->nHeight + 1 <= ::ChainActive().Height())
            pindex = ::ChainActive().Next(pindex);
        else
            return false;
    }
}

CDataStream CZStake::GetUniqueness()
{
    //The unique identifier for a zWAGERR is a hash of the serial
    CDataStream ss(SER_GETHASH, 0);
    ss << hashSerial;
    return ss;
}

bool CZStake::CreateTxIn(std::shared_ptr<CWallet> pwallet, CTxIn& txIn, uint256 hashTxOut)
{
    return false;
/*
    CBlockIndex* pindexCheckpoint = GetIndexFrom();
    if (!pindexCheckpoint)
        return error("%s: failed to find checkpoint block index", __func__);

    CZerocoinMint mint;
    if (!pwallet->GetMintFromStakeHash(hashSerial, mint))
        return error("%s: failed to fetch mint associated with serial hash %s", __func__, hashSerial.GetHex());

    if (libzerocoin::ExtractVersionFromSerial(mint.GetSerialNumber()) < 2)
        return error("%s: serial extract is less than v2", __func__);

    CZerocoinSpendReceipt receipt;
    if (!pwallet->MintToTxIn(mint, hashTxOut, txIn, receipt, libzerocoin::SpendType::STAKE, pindexCheckpoint))
        return error("%s", receipt.GetStatusMessage());

    return true;
*/
}

bool CZStake::CreateTxOuts(std::shared_ptr<CWallet> pwallet, std::vector<CTxOut>& vout, CAmount nTotal)
{
    return false;
/*
    //Create an output returning the zWAGERR that was staked
    CTxOut outReward;
    libzerocoin::CoinDenomination denomStaked = libzerocoin::AmountToZerocoinDenomination(this->GetValue());
    CDeterministicMint dMint;
    if (!pwallet->CreateZWGROutPut(denomStaked, outReward, dMint))
        return error("%s: failed to create zWAGERR output", __func__);
    vout.emplace_back(outReward);

    //Add new staked denom to our wallet
    if (!pwallet->DatabaseMint(dMint))
        return error("%s: failed to database the staked zWAGERR", __func__);

    CAmount toMint = (nTotal - this->GetValue()) / 2;
    CAmount nRemaining = 0;
    while (toMint >= 1 * COIN) {
        libzerocoin::CoinDenomination denomination = libzerocoin::AmountToClosestDenomination(toMint, nRemaining);

        if (denomination == libzerocoin::ZQ_ERROR)
            return error("%s: failed to create zWAGERR output", __func__);

        CAmount nValueNewMint = libzerocoin::ZerocoinDenominationToAmount(denomination);
        toMint -= nValueNewMint;

        CTxOut out;
        CDeterministicMint dMintReward;
        if (!pwallet->CreateZWGROutPut(denomination, out, dMintReward))
            return error("%s: failed to create zWAGERR output", __func__);
        vout.emplace_back(out);

        if (!pwallet->DatabaseMint(dMintReward))
            return error("%s: failed to database mint reward", __func__);
    }

    return true;
*/
}

bool CZStake::GetTxFrom(CTransactionRef& tx)
{
    return false;
}

bool CZStake::GetScriptPubKeyKernel(CScript& scriptPubKeyKernel) const
{
    return false;
}

/*
bool CZStake::MarkSpent(CWallet *pwallet, const uint256& txid)
{
    CxWAGERRTracker* zwgrTracker = pwallet->zwgrTracker.get();
    CMintMeta meta;
    if (!zwgrTracker->GetMetaFromStakeHash(hashSerial, meta))
        return error("%s: tracker does not have serialhash", __func__);

    zwgrTracker->SetPubcoinUsed(meta.hashPubcoin, txid);
    return true;
}
*/

//!Wagerr Stake
bool CStake::SetInput(CTransactionRef txPrev, unsigned int n)
{
    this->txFrom = txPrev;
    this->nPosition = n;
    return true;
}

bool CStake::GetTxFrom(CTransactionRef& tx)
{
    tx = txFrom;
    return true;
}

bool CStake::GetScriptPubKeyKernel(CScript& scriptPubKeyKernel) const
{
    scriptPubKeyKernel = txFrom->vout[nPosition].scriptPubKey;
    return true;
}

bool CStake::CreateTxIn(std::shared_ptr<CWallet> pwallet, CTxIn& txIn, uint256 hashTxOut)
{
    txIn = CTxIn(txFrom->GetHash(), nPosition);
    return true;
}

CAmount CStake::GetValue() const
{
    return txFrom->vout[nPosition].nValue;
}

bool CStake::CreateTxOuts(std::shared_ptr<CWallet> pwallet, std::vector<CTxOut>& vout, CAmount nTotal)
{
    std::vector<valtype> vSolutions;
    CScript scriptPubKeyKernel = txFrom->vout[nPosition].scriptPubKey;
    txnouttype whichType = Solver(scriptPubKeyKernel, vSolutions);
    if (whichType == TX_NONSTANDARD) {
        LogPrintf("CreateCoinStake : failed to parse kernel\n");
        return false;
    }

    if (whichType != TX_PUBKEY && whichType != TX_PUBKEYHASH) {
        LogPrintf("CreateCoinStake : wrong type\n");
        return false; // only support pay to public key and pay to address
    }

    CScript scriptPubKey;
    if (whichType == TX_PUBKEYHASH) // pay to address type
    {
        auto spk_man = pwallet->GetLegacyScriptPubKeyMan();
        if (!spk_man) {
            return false;
        }

        //convert to pay to public key type
        CKey key;
        CKeyID keyID = CKeyID(uint160(vSolutions[0]));
        if (!spk_man->GetKey(keyID, key)) {
            LogPrintf("CreateCoinStake : key not found\n");
            return false;
        }

        scriptPubKey << ToByteVector(key.GetPubKey()) << OP_CHECKSIG;
    } else
        scriptPubKey = scriptPubKeyKernel;

    vout.emplace_back(CTxOut(nTotal, scriptPubKey));

    return true;
}

bool CStake::GetModifier(uint64_t& nStakeModifier)
{
    if (this->nStakeModifier == 0) {
        // look for the modifier
        GetIndexFrom();
        if (!pindexFrom)
            return error("%s: failed to get index from", __func__);
        // TODO: This method must be removed from here in the short terms.. it's a call to an static method in kernel.cpp when this class method is only called from kernel.cpp, no comments..

        if (!GetKernelStakeModifier(pindexFrom->GetBlockHash(), this->nStakeModifier, this->nStakeModifierHeight, this->nStakeModifierTime, false))
            return error("CheckStakeKernelHash(): failed to get kernel stake modifier");
    }
    nStakeModifier = this->nStakeModifier;

    return true;
}

CDataStream CStake::GetUniqueness()
{
    //The unique identifier for a stake is the outpoint
    CDataStream ss(SER_NETWORK, 0);
    ss << nPosition << txFrom->GetHash();
    return ss;
}

//The block that the UTXO was added to the chain
CBlockIndex* CStake::GetIndexFrom()
{
    if (pindexFrom)
        return pindexFrom;
    uint256 hashBlock;
    CTransactionRef tx = GetTransaction(nullptr, nullptr, txFrom->GetHash(), Params().GetConsensus(), hashBlock, true);
    if (!tx) {
        LogPrintf("%s : failed to find tx %s\n", __func__, txFrom->GetHash().GetHex());
        return nullptr;
    } else {
        // If the index is in the chain, then set it as the "index from"
        CBlockIndex* pindex = LookupBlockIndex(hashBlock);
        if (pindex) {
            if (::ChainActive().Contains(pindex))
                pindexFrom = pindex;
        }
    }
    return pindexFrom;
}

bool IsValidStakeInput(const CTxOut& txOut) {
    std::vector<valtype> vSolutions;
    CScript scriptPubKeyKernel = txOut.scriptPubKey;
    txnouttype whichType = Solver(scriptPubKeyKernel, vSolutions);
    if (whichType == TX_NONSTANDARD) {
        return false;
    }

    if (whichType != TX_PUBKEY && whichType != TX_PUBKEYHASH) {
        return false; // only support pay to public key and pay to address
    }
    return true;
}