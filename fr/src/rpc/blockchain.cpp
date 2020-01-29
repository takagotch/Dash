
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

}







