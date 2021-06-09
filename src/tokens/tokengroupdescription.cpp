// Copyright (c) 2019 The ION Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "tokens/tokengroupdescription.h"
#include "util.h"
#include <rpc/protocol.h>
#include <rpc/server.h>
#include <univalue.h>

void CTokenGroupDescriptionRegular::ToJson(UniValue& obj) const
{
    obj.clear();
    obj.setObject();
    obj.pushKV("ticker", strTicker);
    obj.pushKV("name", strName);
    obj.pushKV("document_URL", strDocumentUrl);
    obj.pushKV("documentHash", documentHash.ToString());
    obj.pushKV("decimal_pos", (int)nDecimalPos);
}

void CTokenGroupDescriptionMGT::ToJson(UniValue& obj) const
{
    obj.clear();
    obj.setObject();
    obj.pushKV("ticker", strTicker);
    obj.pushKV("name", strName);
    obj.pushKV("document_URL", strDocumentUrl);
    obj.pushKV("documentHash", documentHash.ToString());
    obj.pushKV("decimal_pos", (int)nDecimalPos);
    obj.pushKV("bls_pubkey", blsPubKey.ToString());
}

bool ParseGroupDescParamsRegular(const JSONRPCRequest& request, unsigned int &curparam, std::shared_ptr<CTokenGroupDescriptionRegular>& tgDesc, bool &confirmed)
{
    std::string strCurparamValue;

    std::string strTicker;
    std::string strName;
    std::string strDocumentUrl;
    uint256 documentHash;

    uint8_t nDecimalPos;

    confirmed = false;

    strTicker = request.params[curparam].get_str();
    if (strName.size() > 10) {
        std::string strError = strprintf("Ticker %s has too many characters (10 max)", strName);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        throw JSONRPCError(RPC_INVALID_PARAMS, "Missing parameter: token name");
    }
    strName = request.params[curparam].get_str();
    if (strName.size() > 30) {
        std::string strError = strprintf("Name %s has too many characters (30 max)", strName);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        std::string strError = strprintf("Not enough paramaters");
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }
    strDocumentUrl = request.params[curparam].get_str();
    if (strDocumentUrl.size() > 98) {
        std::string strError = strprintf("URL %s has too many characters (98 max)", strDocumentUrl);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        // If you have a URL to the TDD, you need to have a hash or the token creator
        // could change the document without holders knowing about it.
        throw JSONRPCError(RPC_INVALID_PARAMS, "Missing parameter: token description document hash");
    }
    strCurparamValue = request.params[curparam].get_str();
    documentHash.SetHex(strCurparamValue);

    curparam++;
    if (curparam >= request.params.size())
    {
        std::string strError = strprintf("Not enough paramaters");
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }
    strCurparamValue = request.params[curparam].get_str();
    int32_t nDecimalPos32;
    if (!ParseInt32(strCurparamValue, &nDecimalPos32) || nDecimalPos32 > 16 || nDecimalPos32 < 0) {
        std::string strError = strprintf("Parameter %d is invalid - valid values are between 0 and 16", nDecimalPos32);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    tgDesc = std::make_shared<CTokenGroupDescriptionRegular>(strTicker, strName, nDecimalPos32, strDocumentUrl, documentHash);

    curparam++;
    if (curparam >= request.params.size())
    {
        return true;
    }
    if (request.params[curparam].get_str() == "true") {
        confirmed = true;
        return true;
    }
    return true;
}

bool ParseGroupDescParamsMGT(const JSONRPCRequest& request, unsigned int &curparam, std::shared_ptr<CTokenGroupDescriptionMGT>& tgDesc, bool &stickyMelt, bool &confirmed)
{
    std::string strCurparamValue;

    std::string strTicker;
    std::string strName;
    std::string strDocumentUrl;
    uint256 documentHash;

    confirmed = false;
    stickyMelt = false;

    strTicker = request.params[curparam].get_str();
    if (strName.size() > 10) {
        std::string strError = strprintf("Ticker %s has too many characters (10 max)", strName);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        throw JSONRPCError(RPC_INVALID_PARAMS, "Missing parameter: token name");
    }
    strName = request.params[curparam].get_str();
    if (strName.size() > 30) {
        std::string strError = strprintf("Name %s has too many characters (30 max)", strName);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        std::string strError = strprintf("Not enough paramaters");
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }
    strDocumentUrl = request.params[curparam].get_str();
    if (strDocumentUrl.size() > 98) {
        std::string strError = strprintf("URL %s has too many characters (98 max)", strDocumentUrl);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        // If you have a URL to the TDD, you need to have a hash or the token creator
        // could change the document without holders knowing about it.
        throw JSONRPCError(RPC_INVALID_PARAMS, "Missing parameter: token description document hash");
    }
    strCurparamValue = request.params[curparam].get_str();
    documentHash.SetHex(strCurparamValue);

    curparam++;
    if (curparam >= request.params.size())
    {
        std::string strError = strprintf("Not enough paramaters");
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }
    strCurparamValue = request.params[curparam].get_str();
    int32_t nDecimalPos32;
    if (!ParseInt32(strCurparamValue, &nDecimalPos32) || nDecimalPos32 > 16 || nDecimalPos32 < 0) {
        std::string strError = strprintf("Parameter %d is invalid - valid values are between 0 and 16", nDecimalPos32);
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        std::string strError = strprintf("Not enough paramaters");
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }
    strCurparamValue = request.params[curparam].get_str();
    CBLSPublicKey blsPubKey;
    blsPubKey.Reset();
    if (!blsPubKey.SetHexStr(strCurparamValue)) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("bls_pubkey must be a valid BLS public key, not %s", strCurparamValue));
    }

    tgDesc = std::make_shared<CTokenGroupDescriptionMGT>(strTicker, strName, nDecimalPos32, strDocumentUrl, documentHash, blsPubKey);

    curparam++;
    if (curparam >= request.params.size())
    {
        std::string strError = strprintf("Not enough paramaters");
        throw JSONRPCError(RPC_INVALID_PARAMS, strError);
    }
    strCurparamValue = request.params[curparam].get_str();
    if (strCurparamValue == "true") {
        stickyMelt = true;
    }

    curparam++;
    if (curparam >= request.params.size())
    {
        return true;
    }
    if (request.params[curparam].get_str() == "true") {
        confirmed = true;
        return true;
    }
    return true;
}
