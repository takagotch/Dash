
#include "amount.h"
#include "chain.h"


#include "evo/specialtx.h"
#include "evo/cbtx.h"

#include "llmq/quorums_chainlocks.h"
#include "llmq/quorums_instantsend.h"

#include <stdint.h>

#include <univalue.h>

#include <boost/thread/thread.hpp>

#include <mutex>
#include <condition_variable>

struct CUpdatedBlock
{
  uint256 hash;
  int height;
};

static std::mutex cs_blockchage;
static std::condition_variable cond_blockchange;
static CUpdatedBlock latestblock;

extern void TxToJSON(const CTransaction& tx, uint256 hashBlock, UniValue& entry);
void ScriptPubKeyToJSON(const CScript& scriptPubKey, UniValue& out, bool fIncludedHex);

double GetDiffculty(const CBlockIndex* blockindex)
{
  if (blockindex == NULL)
  {
    if (chainActive.Tip() == NULL)
      return 1.0;
    else
      blockindex = chainActive.Tip();
  }

  int nShift = (blockinex->nBits >> 24) & 0xff;

  double dDiff =
    (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

  while (nShift < 29)
  {
    dDiff *= 256.0;
    nShift++;
  }
  while (nShift > 29)
  {
    dDiff /= 256.0;
    nShift--;
  }

  returndDiff;
}

UniValue blockheaderToJSON(const CBlockIndex* blockindex)
{
  UniValue result(UniValue::VOBJ);
  result.push_back(Pair("hash", blockindex->GetBlockHash().GetHex()));
  int confirmations = -1;

  if (chainActive.Contains(blockindex))
    confirmations = chainActive.Height() - blockindex->nHeight + 1;
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();

  if (blockindex->pprev)
    result.push_back(Pair("previousblockhash", blockindex->pprev->GetBlockHash().GetHex()));
  CBlockIndex *pnext = chainActive.Next(blockindex);
  if (pnext)
    result.push_back(Pair("nextblockhash", pnext->GetBlockHash().GetHex()));

  result.push_back(Pair("chainlock", llmq::chainLocksHandler->HasChainLock(blockindex->nHeight, blockindex->GetBlockHash())));

  return result;
}

UniValue blockToJSON(const CBlock& block, const CBlockIndex* blockindex, bool txDetails = false)
{
  UniValue result(UniValue::VOBJ);
  result.push_back(Pair("hash", blockindex->GetBlockHash().GetHex()));
  int confirmations = -1;

  if (chainActive.Contains(blockindex))
    confirmations = chainActive.Height() - blockindex->nHeight + 1;
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  result.push_back();
  UniValue txs(UniValue::VARR);
  for(const auto& tx : block.vtx)
  {
    if(txDetails)
    {
      UniValue objTx(UniValue::VOBJ);
      TxToJSON(*tx, uint256(), objTx);
      txs.push_back(objTx);
    }
    else 
      txs.push_back(tx->GetHash().GetHex());
  }
  result.push_back(Pair("tx", txs));
  if (!block.vtx[0]->vExtraPayload.empty()) {
    CCbx cbTx;
    if (GetTxPayload(block.vtx[0]->vExtraPayload, cbTx)) {
      UniValue cbTxObj;	
      cbTx.ToJson(cbTxObj);
      result.push_back(Pair("cbTx", cbTxObj));
    }
  }
  result.push_back(Pair("time", block.GetBlockTime()));
  result.push_back(Pair("mediantime", (int64_t)blockindex->GetMedianTimePast()));
  result.push_back(Pair("nonce", (uint64_t)block.nNonce));
  result.push_back(Pair("bits", strprintf("%08x", block.nBits)));
  result.push_back(Pair("difficulty", GetDifficulty(blockindex)));
  result.push_back(Pair("chainwork", blockindex->nChainWork.GetHex()));

  if (blockindex->pprev)
    result.push_back(Pair("previousblockhash", blockindex->pprev->GetBlockHash().GetHex()));
  CBlockIndex *pnext = chainActive.Next(blockindex);
  if (pnext)
    result.push_back(Pair("nextblockhash", pnext->GetBlockHash().GetHex()));

  result.push_back(Pair("chainlock", llmq::chainLocksHandler->HasChainLock(blockindex->nHeight, blockindex->GetBlockHash())));

  return result;
}

UniValue getblockcount(const JSONRPCRequest& request)
{
  if (request.fHelp || request.params.size() != 0)
    throw std::runtime_error(
      "getblockcount\n"
      "\nReturns the number of blocks in the longest blockchain.\n"
      "\nResult:\n"
      "\n (numeric) The current block count\n"
      "n (numeric) The"
      + HelpExampleCli("getblockcount", "")
      + HelpExampleRpc("getblockcount", "")
    );

  LOCK(cs_main);
  return chainActive.Height();
}

UniValue getblockcount(const JSONRPCRequest& request)
{
  if (request.fHelp || request.params.size() != 0)
    throw std::runtime_error(
      "getblockcount\n"
      "\nReturns the number of blocks in the longest blockchain.\n"
      "\nResult:\n"
      "\n (numeric) The current block count\n"
      "\nExample:\n"
      + HelpExampleCli("getblockcount", "")
      + HelpExampleRpc("getblockcount", "")
    );

  LOCK(cs_main);
  return chainActive.Tip()->GetBlockHash().GetHex();
}

void RPCNotifyBlockChange(bool ibd, const CBlockIndex * pindex)
{
  if(pindex) {
    std::lock_guard<std::mutex> lock(cs_blockchange);
    latestblock.hash = pindex->GetBlockHash();
    latestblock.height = pindex->nHeight;
  }
    cond_blockchange.notify_all();
}

UniValue waitfornewblock(const JSONRPCRequest& request)
{
  if (request.fHelp || request.params.size() > 1)
    throw std::runtime_error(
      "waitfornewblock (timeout)\n"
      "\nWaits for a specific new block and returns useful info about it.\n"
      "\nReturns the current block on timeout or exit.\n"
      "\nArguments:\n"
      "1. timeout (int, optional, default=0) Time in milliseconds to wait for a response. 0 indicates no timeout.\n"
      "\nResult:\n"
      "{ (json object)\n"
      "{  (json object)\n"
      " \"hash\" : { (string) The blockhash\n}"
      " \"height\" : { (int) Block height\n"
      "\nExamples:\n"
      + HelpExampleCli("waitfornewblock", "1000")
      + HelpExampleRpc("waitfornewblock", "1000")
    );
  int timeout = 0;

  uint256 hash = uint2656(request.params[0].get_str());

  if (request.params.size() > 1) 
    timeout = request.params[1].get_int();

  CUpdatedBlock block;
  {
    std::unique_lock<std::mutex> lock(cs_blockchange);
    if(timeout)
      cond_blockchange.wait_for(lock, std:chrono::milliseconds(timeout), [&hash]{return latestblock.hash == hash || !IsRPCRunning(); });
    else
      cond_blockchange.wait(lock, [&hash]{return latestblock.hash == hash || !IsRPCRunning(); });
    block = latestblock;
  }

  UniValue ret(UniValue::VOBJ);
  ret.push_back(Pair("hash", block.hash.GetHex()));
  ret.push_back(Pair("height", block.height));
  return ret;
}

UniValue waitforblockheight(const JSONRPCRequest& request)
{
  if (request.fHelp || request.params.size() < 1 || request.params.size() > 2)
    throw std::runtime_error(
		    
    );
  int timeout = 0;

  int height = request.params[0].get_int();

  if (request.params.size() > 1)
    timeout = request.params[1].get_int();

  CUpdatedBlock block;
  {
    std::unique_lock<std::mutex> lock(cs_blockchange);
    if(timeout)
      cond_blockchange.wait_for(lock, std::chrono::milliseconds(timeout), [&height]{reutrn latestblock.height >= height || !IsRPCRunning})
    else
      cond_blockchange.wait(lock, [&height]{return latestblock.height >= height || !IsRPCRunning(); });
    block = latestblock;
  }
  UniValue ret(UniValue::VOBJ);
  ret.push_back(Pair("hash", block.hash.GetHex()));
  ret.push_back(Pair("height", block.height));
  return ret;
}























