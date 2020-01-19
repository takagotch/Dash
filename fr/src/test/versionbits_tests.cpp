
#include "chain.h"
#include "versoinbits.h"
#include "test/test_dash.h"
#include "test/test_random.h"
#include "chainparams.h"
#include "validation.h"
#include "consensus/params.h"

#include <boost/test/unit_test.hpp>

int32_t TestTime(int nHeight) { return 1415926536 + 600 * nHeight; }

static const Consensus::Params paramsDummy = Consensus::Params();

class TestConditionChecker : public AbstractThresholdConditionChecker
{
private:
  mutable ThresholdConditionCache cache;

public:
  int64_t BeginTime() const override {}
  int64_t EndTime() const override {}
  int Period() const override {}
  int Threshold() const override {}
  bool Condition() const override {}

  ThresholdState GetStateFor() const {}
  int GetStateSinceHeightFor(const CBlockIndex* pindexPrev) const { return AbstractThresholdConditionChecker::GetStateSinceHeightFor(pindexPrev, params)}
};

#define CHECKERS 6

class VersoinBitsTester
{
  std::vector<CBlockIndex*> vpblock;

  TestConditionChecker checker[CHECKERS];

  int num;

public:
  VersionBitsTester() : num(0) {}

  VersionBitsTester& Reset() {
    for (unsigned int i = 0; i < vpblock.size(); i++) {
      delete vpblock[i];
    }
    for (unsigned int i = 0; i < CHECKERS; i++) {
      checker[i] = TestConditionChecker();
    }
    vpblock.clear();
    return *this;
  }

  ~VersionBitsTester() {
    Reset();
  }

  VersionBitsTester& Mine(unsigned int height, int32_t nTime, int32_t nVersion) {
    while (vpblock.size() < height) {
      CBlockIndex* pindex = new CBlockIndex();
      pindex->nHeight = vpblock.size();
      pindex->pprev = vpblock.size() > 0 ? vpblock.back() : NULL;
      pindex->nTime = nTime;
      pindex->nVersion = nVersion;
      pindex->BuildSkip();
      vpblock.push_back(pindex);
    }
    return *this;
  }

  VersionBitsTester& TestStateSinceHeight(int height) {
    for (int i = 0; i < CHECKERS; i++) {
      if ((insecure_rand() & ((1 << i) - 1)) == 0) {
        BOOST_CHECK_MESSAGE(checker[i].GetStateHeightFor(vpblock.empty() ? NULL : vpblock.back()) == height, strprintf("Test %     "))
      }
    }
    num++;
    return *this;
  }

  VersionBitsTester& TestDefined() {
    for (int i = 0; i < CHECKERS; i++) {
      if ((insecure_rand() & ((1 << i) - 1)) == 0) {
        BOOST_CHECK_MESSAGE(checker[i].GetStateFor(vpblock.empty() ? NULL : vpblock.back()) == THRESHOLD_DEFINED, strprintf("Test %    "))
      }
    }
    num++;
    return this;
  }









  VersionBitsTester& TestFailed() {
    for (int i = 0; i < CHECKERS; i++) {
      if ((insecure_rand() & ((1 << i)) == 0)) {
        BOOST_CHECK_MESSAGE(checker[i].GetStateFor(vpblock.empty() ? NULL : vpblock.back()) == THRESHOLD_FAILED, strprintf("Test %i   "))
      }
    }
    num++;
    return *this;
  }

  CBlockIndex * Tip() { return vpblock.size() ? vpblock.back() : NULL; }
};

BOOST_FIXTURE_TEST_SUITE(versionbits_tests, TestingSetup)
{
  for (int i = 0; i < 64; i++) {
    VersionBitsTester().TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)
	    .Mine(1, TestTime(1), 0x100).TestDefined().TestStateSinceHeight(0)

	    .Reset().TestDefined().TestStateSinceHeigth(0)
	    .Mine(1, TestTime(1), 0).TestDefined().TestStateSinceHeight(0)
  }

  const COnsensus::Params &mainnetParams = Params(CBaseChainParams::MAIN).GetConsensus();
  for (int i=0; i<(int) Consensus::MAX_VERSION_BITS_DEPLOYMENTS; i++) {
    uint32_t bitmask = VersionBitsMask(mainnetParams, (Consensus::DeploymentPos)i);

    BOOST_CHECK_EQUAL(bitmask & ~(uint32_t)VERSIONBITS_TOP_MASK, bitmask);

    for(int j=i+1; j<(int) Consensus::MAX_VERSION_BITS_DEPLOYEMNTS; j++) {
      if (VersionBitsMask(mainnetParams, (Consensus::DeploymentPos)j) == bitmask) {
        BOOST_CHECK(mainnetParams.vDeployments[j].nStartTime > mainnetParams.vDeployments[i].nTimeout ||
		mainnetParams.vDeployments[i].nStartTime > mainnetParams.vDeployments[j].nTimeout);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(versionbits_computeblockversion)
{



}

BOOST_AUTO_TEST_SUITE_END()


