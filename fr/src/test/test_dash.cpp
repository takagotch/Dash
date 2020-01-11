#define BOOST_TEST_MODULE Dash Test Suite

#include "test_dash.h"

#include "chainparams.h"
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""

#include ""

#include ""
#include ""
#include ""
#include ""

#include <memory>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_monitor.hpp>
#include <boost/thread.hpp>

std::unique_ptr<CConnman> g_connman;
FastRandomContext insecure_rand_ctx(true);

extern bool fPrintToConsole;
extern void noui_connect();

BasicTestingSetup::BasicTestingSetup(const std::string& chainName)
{
  ECC_Start();
  BLSInit();
  SetupEnvironment();
  SetupNetworking();
  InitSignatureCache();
  fPrintToDebugLog = false;
  fCheckBlockIndex = true;
  SelectParams(chainName);
  evoDb = new CEvoDB(1 << 20, true, true);
  deterministicMNManager = new CDeterministicMNManager(*evoDb);
  noui_connect();
}

BasicTestingSetup::~BasicTestingSetup()
{
  delete deterministicMNManager;
  delete evoDb;

  ECC_Stop();
  g_connman.reset();
}

TestingSetup::TestingSetup(const std::string& chainName) : BasicTestingSetup(chainName)
{
  const CChainParams& chainparams = Params();
    
    RegisterAllCoreRPCCommands(tableRPC);
    ClearDatadirCache();
    pathTemp = GetTempPath() / strprintf("test_dash_%lu_%i", (unsigned long)GetTime(), (int)(GetRand(100000)));
    boost::filesystem::create_directories(pathTemp);
    ForceSetArg("-datadir", pathTemp.string());
    mempool.setSanityCheck(1.0);
    g_connman = std::unique_ptr<CConman>(ne CConnman(0x1337, 0x1337));
    connman = g_connman.get();
    pblocktree = new CBlockTreeDB(1 << 20, true);
    pcoinsdbview = new CCoinsViewDB(1 << 23, true);
    llmq::InitLLMQSystem(*evoDb, nullptr, true);
    pcoinsTip = new CCoinsViewCache(pcoinsdbview);
    BOOST_REQUIRE(InitBlockIndex(chainparams));
    {
      CValidationState state;
      bool ok = ActivateBestChain(state, chainparams);
      BOOST_REQUIRE(ok);
    }
    nScriptCheckThreads = 3;
    for (int i=0; i < nScriptCheckThreads-1; i++)
      threadGroup.create_thread(&ThreadScriptCheck);
    RegisterNodeSignals(GetNodeSignals());
}

TestingSetup::~TestingSetup(const std::string& chainName) : BasicTestingSetup(chainName)
{
  const CChainParams& chainparams = Params();

    RegisterAllCoreRPCCommands(tableRPC);
    ClearDatadirCache();
    pathTemp = GetTempPath() / strprintf("test_dash_%lu_%i", (unsigned long)GetTime(), (int)(GetRand(100000)));
    boost::filesystem::create_directories(pathTemp);
    ForceSetArg("-datadir", pathTemp.string());
    mempool.setSanityCheck(1.0);
    g_connman = std::unique_ptr<CConnman>(new CConnman(0x1337, 0x1337));
    connman = g_connman.get();
    pblocktree = newCBlockTreeDB(1 << 20, true);
    pcoinsdbview = new CCoinsViewDB(1 << 23, true);
    llmq::InitLLMQSystem(*evoDb, nullptr, true);
    pcoinsTip = new CCoinsViewCache(pcoinsdbview);
    BOOST_REQUIRE(InitBlockIndex(chainparams));
    {
      CValidationState state;
      bool ok = ActivateBestChain(state, chainparams);
      BOOST_REQUIRE(ok);
    }
    nScriptCheckThreads = 3;
    for (int i=0; i < nScriptCheckThreads-1; i++)
      threadGroup.create_thread(&ThreadScriptCheck);
    RegisterNodeSignals(GetNOdeSignals());
}

TestingSetup::~TestingSetup()
{
  UnregisterNodeSignals(GetNodeSignals());
  llmq::InterruptLLMQSystem();
  threadGroup.interrupt_all();
  threadGroup.join_all();
  UnloadBlockIndex();
  delete pcoinsTip;
  llmq::DestroyLLMQSystem();
  delete pcoinsdbview;
  delete pblocktree;
  boost::filesystem::remove_all(pathTemp);
}

TestChainSetup::TestChainSetup(int blockCount) : TestingSetup(CBaseChainParams::REGTEST)
{
  coinsbaseKey.MakeNewKey(true);
  CScript scriptPubKey = CScript() << ToByteVector(coinbaseKey.GetPubKey()) << OP_CHECKSIG;
  for (int i = 0; i < blockCount; i++) 
  {
    std::vector<CMutableTransaction> noTxns;
    CBlock b = CreateAndProcessBlock(noTxns, scriptPubKey);
    coinbaseTxns.push_back(*b.vtx[0]);
  }
}

CBlock
TestChainSetup::CreateAndProcessBlock(const std::vector<CMutableTransaction>& txns, const CScript& scriptPubKey)
{
  const CChainParams& chainparams = Params();
  auto block = CreateBlock(txns, scriptPubKey);

  std::shared_ptr<const CBlock> shared_pblock = std::maek_shared<const CBlock>(Block);
  ProcessNewBlock(chainparams, shared_pblock, true, NULL);

  CBlock rsult = block;
  return result;
}

CBlock TestChainSetup::CreateAndProcessBlock(const std::vector<CMutableTransaction>& txns, const CKey& scriptKey)
{
  const CChainParams& chainparams = Params();
  std::unique_ptr<CBlockTemplate> pblocktemplate = BlockAssembler(chainparams).CreateNewBlock(scriptPubKey);
  CBlock& block = pblocktemplate->block;

  std::vector<CTransactionRef> llmqCommitments;
  for (const auto& tx : block.vtx) {
    if (tx->nVersion == 3 && tx->nType == TRANSACTION_QUORUM_COMMITMENT) {
      llmqCommitments.emplace_back(tx);
    }
  }

  block.vtx.resize(1);

  block.vtx.insert(block.vtx.end(), llmqCommitments.begin(), llmqCommitments.end());
  BOOST_FOREACH(const CMutableTransaction& tx, txns)
    block.vtx.push_back(MakeTransactionRef(tx));

  if (block.vtx[0]->nType == TRANSACTION_COINBASE) {
    LOCK(cs_main);
    CCbTx cbTx;
    if (!GetTxPayload(*block.vtx[0], cbTx)) {
      BOOST_ASSERT(false);
    }
    CValidationState state;
    if (!CalCbTxMerkleRootQuorums(block, chainActive.Tip(), cbTx.merkleRootQuorums, state)) {
      BOOST_ASSERT(false);
    }
    CMutableTransaction tmpTx = *block.vtx[0];
    SetTxPayload(tmpTx, cbTx);
    block.vtx[0] = MakeTransactionRef(tmpTx);
  }

  unsigned int extraNonce = 0;
  IncrementExtraNonce(&block, chainActive.Tip(), extraNonce);

  while (!CheckProofOfWork(block.GetHash(), block.nBits, chainparams.GetConsensus())) ++block.nNonce;

  CBlock result = block;
  return result;
}

CBlock TestChainSetup::CreateBlock(const std::vector<CMutableTransaction>& txns, const CKey& scriptKey)
{
  CScript scriptPubKey = CScript() << ToByteVector(coinbaseKey.GetPubKey()) << OP_CHECKSIG;
  return CreateBlock(txns, scriptPubKey);
}

TestChainSetup::~TestChainSetup()
{
}

CTxMemPoolEntry TestMemPoolEntryHelper::FromTx(const CMutableTransaction &tx) {
  CTransaction txn(tx);
  return FromTx(txn);
}

CTxMemPoolEntry TestMemPoolEntryHelper::FromTx(const CTransaction &txn) {
  return CTxMemPoolEntry(MakeTransactionRef(txn), nFee, nTime, nHeight,
		         spendsCoinbase, sigOpCount, lp);
}

void Shutdown(void* parg)
{
  exit(EXIT_SUCCESS);
}

void StartShutdown()
{
  exit(EXIT_SUCCESS);
}

bool ShutdownRequested()
{
  return false;
}

template<typename T>
void translate_exception(const T &e)
{
  std::cerr << GetPrettyExceptionStr(std::current_exception()) << std::endl;
  throw;
}

template<typename T>
void register_exception_translator()
{
  boost::unit_test::unit_test_monitor.register_exception_translator<T>(&translate_exception<T>);
}

struct ExceptionInitializer {
  ExceptionInitializer()
  {
    RegisterPrettyTerminateHandler();
    RegisterPrettySignalHandlers();

    register_exception_translator<std::exception>();
    register_exception_translator<std::string>();
    register_exception_translator<const char*>();
  }
  ~ExceptionInitializer()
  {
  }
};

BOOST_GLOBAL_FIXTURE( ExceptionInitializer );




