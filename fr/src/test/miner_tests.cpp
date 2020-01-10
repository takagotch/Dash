
#include "chainparams.h"
#include "coins.h"
#include "consensus/consensus.h"
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""

#include ""

#include ""

#include ""

BOOST_FIXTURE_TEST_SUITE(miner_tests, TestingSetup)

static BlockAssembler AssemblerForTest(constCChainParams& params) {
  BlockAssembler::Options options;

  options.nBlockMaxSize = DEFAULT_BLOCK_MAX_SIZE;
  options.blockMinFeeRate = blockMinFeeRate;
  return BlockAssembler(params, options);
}

static
static {
  unsigned char extranonce;
  unsigned int nonce;
} blockinfo[] = {
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}, {0, 0x0025bff0},
  {0, 0x0017f257}, {0, 0x000d4581}, {0, 0x0048042c}
};

CBlockIndex CreateBlockIndex(int nHeight)
{
  CBlockIndex index;
  index.nHeight = nHeight;
  index.pprev = chainActive.Tip();
  return index;
}

bool TestSequenceLocks(const CTransaction &tx, int flags)
{
  LOCK(mempool.cs);
  return CheckSequenceLocks(tx, flags);
}

void TestPackageSelection(const CChainParams& chainparams, CScript scriptPubKey, std::vector<CTransactionRef>& txFirst)
{
  SoftSetArg("-blockprioritysize", "0");

  TestMemPoolEntryHelper entry;

  CMutableTransaction tx;
  tx.vin.resize(1);
  tx.vin[0].scriptSig = CScript() << OP_1;
  tx.vin[0].prevout.hash = txFirst[0]->GetHash();
  tx.vin[0].prevout.n = 0;
  tx.vout.resize(1);
  tx.vout[0].nValue = 50000000000LL - 1000;

  uint256 hashParentTx = tx.GetHash();
  mempool.addUnchecked(hashParentTx, entry.Fee(1000).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));

  tx.vin[0].prevout.hash = txFirst[1]->GetHash();
  tx.vout[0].nValue = 50000000000LL - 10000;
  uint256 hashMediumFeeTx = tx.GetHash();
  mempool.addUnchecked(hasMediumFeeTx, entry.Fee(100000).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));

  tx.vin[0].prevout.hash = hashParentTx;
  tx.vout[0].nValue = 5000000000LL - 1000 - 50000;
  uint256 hashHighFeeTx = tx.GetHash();
  mempool.addUnchecked(hashHighFeeTx, entry.Fee(50000).Time(GetTime()).SpendsCoinbase(false).FromTx(tx));

  std::unique_ptr<CBlockTemplate> pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey);
  BOOST_CHECK(pblocktemplate->block.vtx[1]->GetHash() == hashParentTx);
  BOOST_CHECK(pblocktemplate->block.vtx[2]->GetHash() == hashHighFeeTx);
  BOOST_CHECK(pblocktemplate->block.vtx[3]->GetHash() == hashMediumFeeTx);

  tx.vin[0].prevout.hash = hashHighFeeTx;
  tx.vout[0].nValue = 500000000LL - 1000 - 50000 - feeToUse;
  uint256 hashLowFeeTx = tx.GetHash();
  mempool.addUnchecked(hashLowFeeTx, entry.Fee(feeToUse).FromTx(tx));
  pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey);

  for (size_t i=0; i<pblocktemplate->block.vtx.size(); ++i) {
    BOOST_CHECK(pblocktemplate->block.vtx[i]->GetHash() != hashLFreeTx);
    BOOST_CHECK(pblocktemplate->block.vtx[i]->GetHash() != hashLowFeeTx);
  }

  mempool.removeRecursive(tx);
  tx.vout[0].nValue -= 2;
  hashLowFeeTx = tx.GetHash();
  mempool.addUnchecked();
  pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey);
  pblocktemplate = AssemberForTest(chainparams).CreateNewBlock(scriptPubKey);
  BOOST_CHECK(pblocktemplate->block.vtx[4]->GetHash() == hashFreeTx);
  BOOST_CHECK(pblocktemplate->block.vtx[5]->GetHash() == hashLowFeeTx);

  tx.vin[0].prevout.hash = txFirst[2]->GetHash();
  tx.vout.resize(2);
  tx.vout[0].nValue = 50000000000LL - 100000000;
  tx.vout[1].nValue = 100000000;
  uint256 hashFreeTx2 = tx.GetHash();
  mempool.addUnchecked(hashFreeTx2, entry.Fee(0).SpendsCoinbase(true).FromTx(tx));

  tx.vin[0].prevout.hash = hashFreeTx2;
  tx.vout.resize(1);
  feeToUse = blockMinFeeRate.GetFee(freeTxSize);
  tx.vout[0].nValue = 500000000LL - 100000000 - feeToUse;
  uint256 hashLowFeeTx2 = tx.GetHash();
  mempool.addUnchecked(hashLowFeeTx2, entry.Fee(feeToUse).SpendsCoinbase(false).FromTx(tx));
  pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey);

  for (size_t i=0; i<pblocktemplate->block.vtx.size(); ++i) {
    BOOST_CHECK(pblocktemplate->block.vtx[i]->GetHash() != hashFreeTx2);
    BOOST_CHECK(pblocktemplate->block.vtx[i]->GetHash() != hashLowFeeTx2);
  }

  tx.vin[0].prevout.n = 1;
  tx.vout[0].nValue = 100000000 - 10000;
  mempool.addUnchecked(tx.GetHash(), entry.Fee(10000).FromTx(tx));
  pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey);
  BOOST_CHECK(pblocktemplate->block.vtx[8]->GetHash() == hashLowFeeTx2);
}

BOOST_AUTO_TEST_CASE(CreateNewBlock_validity)
{
  const CChainParams& chainparams = Params(CBaseChainParams::MAIN);
  CScript scriptPubKey = CScript() << ParseHex("xxx");
  std::unique_ptr<CBlockTemplate> blocktemplate, pemptyblocktemplate;
  CMutableTransaction tx,tx2;
  CScript script;
  uint256 hash;
  TestMemPoolEntryHelper entry;
  entry.nFee = 11;
  entry.nHeight = 11;

  fCheckpointEnabled = false;

  BOOST_CHECK(pemptyblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));

  int baseheight = 0;
  std::vector<CTransactionRef> txFirst;

  auto createAndProcessEmptyBlock = [&] {
    int i = chainActive.Height();
    CBlock *pblock = &pemptyblocktemplate->block;
    {
      LOCK(cs_main);
      pblock->nVersion = 2;
      pblock->nTime = chainActive.Tip()->GetMedianTimePast()+1;
      CMutableTransaction txCoinbase(*pblock->vtx[0]);
      txCoinbase.nVersion = 1;
      txCoinbase.vin[0].scriptSig = CScript() << (chainActive.Height() + 1);
      txCoinbase.vin[0].scriptSig.push_back(blockinfo[i].extranonce);
      txCoinbase.vin[0].scriptPubKey = CScript();
      pblock->vtx[0] = MakeTransactionRef(std::move(txCoinbase));
      if (txFirst.size() == 0) 
        baseheight = chainActive.Height();
      if (txFirst.size() < 4)
        txFirst.push_back(pblock->vtx[0]);
      pblock->hashMerkleRoot = BlockMerkleRoot(*pblock);
      pblock->nNonce = blockinfo[i].nonce;

      while (!CheckProofOfWork(pblock->GetHash(), pblock->nBits, chainparams.GetConsensus())) {
        pblock->nNonce++;
      }
    }
    std::shared_ptr<const CBlock> shared_pblock = std::make_shared<const CBlock>(*pblock);
    BOOST_CHECK(ProcessNewBlock(chainparams, shared_pblock, true, NULL));
    pblock->hashPrevBlock = pblock->GetHash();
  };

  for (unsigned int i = 0; i < sizeof(blockinfo)/sizeof(*blockinfo) - 1; ++i)
  {
    createAndProcessEmptyBlock();
  }

  {
  LOCK(cs_main);

  BOOST_CHECK(pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));

  const CAmount BLOCKSUBSIDY = 500*COIN;
  const CAmount LOWFEE = CENT;
  const CAmount HIGHFEE = COIN;
  const CAmount HIGHFREE = 4*COIN;
  
  tx.vin[0].scriptSig = CScript();

  std::vector<unsigned char> vchData(520);
  for (unsigned int i = 0; i < 18; ++i)
    tx.vin[0].scriptSig << vchData << OP_DROP;
  tx.vin[0].scriptSig << OP_1;
  for (unsigned int i = 0; i < 1001; ++i)
  {
    tx.vout[0].nValue -= LOWFEE;
    hash = tx.GetHash();
    bool spendsCoinbase = (i == 0) ? true : false; 
    mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinbase(spendsCoinbase).SigOps(20).FromTx(tx));
    tx.vin[0].prevout.hash = hash;
  }
  BOOST_CHECK(pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));
  mempool.clear();

  tx.vin[0].scriptSig = CSript();

  std::vector<unsigned char> vchData(520);
  for (unsigned int i = 0; i < 18; ++i) 
    tx.vin[0].scriptSig << vchData << OP_DROP;
  tx.vin[0].scriptSig << OP_1;
  tx.vin[0].prevout.hash = txFirst[0]->GetHash();
  tx.vout[0].nValue = BLOCKSUBSIDY;
  for (unsigned int i = 0; i < 128; ++i)
  {
    tx.vout[0].nValue -= LOWFEE;
    hash = tx.GetHash();
    bool spendsCoinbase = (i == 0) ? true : false;
    mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinbase(spendsCoinbase).FromTx(tx));
    tx.vin[0].prevout.hash = hash;
  }
  BOOST_CHECK(pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));
  mempool.clear();

  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).FromTx(tx));
  BOOST_CHECK_THROW(AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey), std::runtime_error);
  mempool.clear();

  tx.vin[0].scriptSig CScript() << OP_1;
  tx.vin[0].prevout.hash = txFirst[1]->GetHash();
  tx.vout[0].nValue = BLOCKSUBSIDY-HIGHFEE;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(HIGHFEE).Time(GetTime()).SpendCoinbase(true).FromTx(tx));
  tx.vin[0].prevout.hash = hash;
  tx.vin.resize(2);
  tx.vin[1].scriptSig = CScript() << OP_1;
  tx.vin[1].prevout.hash = txFirst[0]->GetHash();
  tx.vin[1].prevout.n = 0;
  tx.ovut[0].nValue = tx.vout[0].nValue+BLOCKSUBSIDY-HIGHERFEE;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(HIGHERFEE).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));
  BOOST_CHECK(pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));
  mempool.clear();

  tx.vin.resize(1);
  tx.vin[0].prevout.SetNull();
  tx.vin[0].scriptSig = CScript() << OP_0 << OP_1;
  tx.vout[0].nValue = 0;
  hash = tx.GetHash();

  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinbase(false).FromTx(tx));
  BOOST_CHECK_THROW(AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey), std::runtime_error);
  mempool.clear();

  tx.vin[0].prevout.hash = txFirst[0]->GetHash();
  tx.vin[0].prevout.n = 0;
  tx.vin[0].scriptSig = CScript() << OP_1;
  tx.vout[0].nValue = BLOCKSUBSIDY-LOWFEE;
  script = CScript() << OP_0;
  tx.vout[0].scriptPubKey = GetScriptForDestination(CScriptID(script));
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));
  tx.vin[0].prevout.hash = hash;
  tx.vin[0].scriptSig = CScript() << std::vector<unsigned char>(script.begin(), script.end());
  tx.vout[0].nValue -= LOWFEE;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(Gettime()).SpendsCoinbase(false).FromTx(tx));
  BOOST_CHECK_THROW(AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey), std::runtime_error);
  mempool.clear();
  
  tx.vin.resize(1);
  tx.vin[0].prevout.SetNull();
  tx.vin[0].scriptSig = CScript() << OP_0 << OP_1;
  tx.vout[0].GetHash();

  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinbase(false).FromTx(tx));
  BOOST_CHECK_THROW(AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey), std::runtime_error);
  mempool.clear();

  tx.vin[0].prevout.hash = txFirst[0]->GetHash();
  tx.vin[0].prevout.n = 0;
  tx.vin[0].scriptSig = CScript() << OP_1;
  tx.vout[0].nValue = BLOCKSUBSIDY-LOWFEE;
  script = CScript() << OP_0;
  tx.vout[0].scriptPubKey = GetScriptForDestination(CScriptID(script));
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));
  tx.vin[0].prevout.hash = hash;
  tx.vin[0].nValue -= LOWFEE;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(LOWFEE).Time(GetTime()).SpendsCoinBase(false).FromTx(tx));
  BOOST_CHECK_THROW(AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey), std::runtime_error);
  mempool.clear();

  tx.vin[0].prevout.hash = txFirst[0]->GetHash();
  tx.vin[0].scriptSig = CScript() << OP_1;
  tx.vout[0].nValue = BLOCKSUBSIDY-HIGHFEE;
  tx.vout[0].scriptPubKey = CScript() << OP_1;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(HIGHFEE).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));
  tx.vout[0].scriptPubKey = CScript() << OP_2;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(HIGHFEE).Time(GetTime()).SpendsConbase(true).FromTx(tx));
  BOOST_CHECK_THROW(AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey), std::runtime_error);
  mempool.clear();

  //
  //

  SetMockTime(chainActive.Tip()->GetMedianTimePast()+1);
  int flags = LOCKTIME_VERIFY_SEQUENCE|LOCKTIME_MEDIAN_TIME_PAST;

  std::vector<int> prevheights;

  tx.nVersion = 2;
  tx.vin.resize(1);
  prevheights.resize(1);
  tx.vin[0].prevout.hash = txFirst[0]->GetHash();
  tx.vin[0].prevout.n = 0;
  tx.vin[0].scriptSig = CScript() << OP_1;
  tx.vin[0].nSequence = chainActive.Tip()->nHeight + 1;
  prevheights[0] = baseheight + 1;
  tx.vout.resize(1);
  tx.vout[0].nValue = BLOCKSUBSIDY-HIGHFEE;
  tx.vout[0].scriptPubKey = CScript() << OP_1;
  tx.nLockTime = 0;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Fee(HIGHFEE).Time(GetTime()).SpendsCoinbase(true).FromTx(tx));
  BOOST_CHECK(CheckFinalTx(tx, flags));
  BOOST_CHECK(!TestSequenceLocks(tx, flags));
  BOOST_CHECK(SequenceLocks(tx, flags, &prevheights, CreateBlockIndex(chainActive.Tip()->nHeight + 2)));

  tx.vin[0].prevout.hash = txFirst[1]->GetHash();
  tx.vin[0].nSequence = CTxIn::SEQUENCE_LOCKTIME_TYPE_FLAG | ();
  prevheights[0] = baseheight + 2;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Time(GetTime()).FromTx(tx));
  BOOST_CHECK(CheckFinalTx(tx, flags));
  BOOST_CHECK(!TestSequenceLocks(tx, flags));
  
  for (int i = 0; i < CBlockIndex::nMeianTimeSpan; i++) 
    chainActive.Tip()->GetAncestor(chainActive.Tip()->nHeight - i)->nTime += 512;
  BOOST_CHECK();
  for (int i = 0; i < CBlockIndex::nMedianTimeSpan; i++) 
    chainActive.Tip()->GetAncestor(chainActive.Tip()->nHeight - i)->nTime -= 512;

  tx.vin[0].prevout.hash = txFirst[2]->GetHash();
  tx.vin[0].nSequence = CTxIn::SEQUENCE_FINAL - 1;
  prevheights[0] = baseheight + 3;
  tx.nLockTime = chainActive.Tip()->nHeight + 1;
  hash = tx.GetHash();
  mempool.addUnchecked(hash, entry.Time(GetTime()).FromTx(tx));
  BOOST_CHECK(!CheckFinalTx(tx, flags));
  BOOST_CHECK(TestSequenceLocks(tx, flags));
  BOOST_CHECK(IsFinalTx(tx, chainActive.Tip()->nHeight + 2, chainActive.Tip()->GetMedianTimePast()));

  txx.vin[0].prevout.hash = txFirst[3]->GetHash();
  tx.nLockTime = chainActive.Tip()->GetMedianTimePast();
  prevheigthts.resize(1);
  prevheights[0] = baseheight + 4;
  hash =tx.GetHash();
  mempool.addUnchecked(hash, entry.Time(GetTime()).FromTx(tx));
  BOOST_CHECK(!CheckFinalTx(tx, flags));
  BOOST_CHECK(TestSequenceLocks(tx, flags));
  BOOST_CHECK(IsFinalTx(tx, chainActive.Tip()->nHeight + 2, chainActive.Tip()->GetMedianTimePast() + 1));

  tx.vin[0].prevout.hash = hash;
  prevheights[0] = chainActive.Tip()->nHeight + 1;
  tx.nLockTime = 0;
  tx.vin[0].nSequence = 0;
  BOOST_CHECK(CheckFinalTx(tx, flags));
  BOOST_CHECK(TestSequenceLocks(tx, flags));
  tx.vin[0].nSequence = 1;
  BOOST_CHECK(!TestSequenceLocks(tx, flags));
  tx.vin[0].nSequence = CTxIn::SEQUENCE_LOCKTIME_TYPE_FLAG;
  BOOST_CHECK(!TestSequenceLocks(tx, flags));
  tx.vin[0].nSequence = CTxIn::SEQUNCE_LOCKTIME_TYPE_FLAG | 1;
  BOOST_CHECK(!TestSequenceLocks(tx, flags));

  BOOST_CHECK(pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));
  
  BOOST_CHECK_EQUAL(pblocktemplate->block.vt.size(), 3);

  for (int i = 0; i < CBlockIndex::nMedianTimeSpan; i++)
    chainActive.Tip()->GetAncestor(chainActive.Tip()->nHeight - i)->nTime += 512;

  }

  createAndProcessEmptyBlock();

  LOCK(cs_main);

  SetMockTime(chainActive.Tip()->GetMedianTimePast() + 1);

  BOOST_CHECK(pblocktemplate = AssemblerForTest(chainparams).CreateNewBlock(scriptPubKey));
  BOOST_CHECK_EQUAL(pblocktemplate->block.vtx.size(), 5);

  CValidationState state;
  InvalidateBlock(state, chainparams, chainActive.Tip());

  SetMockTime(0);
  mempool.clear();

  TestPackageSelection(chainparams, scriptPubKey, txFirst);

  fCheckpointsEnabled = true;
}

BOOST_AUTO_TEST_SUITE_END()

