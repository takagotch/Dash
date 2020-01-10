
#include "test/test_dash.h"

#include "bls/bls.h"
#include "evo/simplifiedmns.h"
#include "netbase.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(evo_simplefiedmns_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(simplifiedmns_merklerroots)
{
  std::vector<CSimplifiedMNSListEntry> entries;
  for (size_t i = 0; i < 15; i++) {
    CSimplifiedMNListEntry smle;
    smle.proRegTxHash.SetHex(strprintf("%064x", i));
    smle.confirmedHash.SetHex(strprintf("%064x", i));

    std::string ip = strprintf("%d.%d.%d.%d", 0, 0, 0, i);
    Lookup(ip.c_str(), smle.service, i, false);

    uint8_t skBuf[CBLSSecretKey::SerSize];
    memset(skBuf, 0, sizeof(skBuf));
    skBuf[0] = (uint8_t)i;
    CBLSSecretKey sk;
    sk.SetBuf(skBuf, sizeof(skBuf));

    smle.pubKeyOperator.Set(sk.GetPublicKey());
    smle.keyIDVoting.SetHex(strprintf("%040x", i));
    smle.isValid = true;

    entries.emplace_back(smle);
  }

  std::vector<std::string> expectedHashes = {
    "xxx",
    "xxx",
    "xxx",
    "xxx",
    "xxx",
    "xxx",
    "xxx",
    "xxx",
    "xxx",
    "xxx",
  };
  std::vector<std::string> calculatedHashes;

  for (auto& smle : entries) {
    calculatedHashes.emplace_back(smle.CalcHash().ToString());
  }

  BOOST_CHECK(expectedHashes == calculatedHashes);

  CSimpleifiedMNList sml(entries);

  std::string expectedMerkleRoot = "xxxx";
  std::string calculatedMerkleRoot = sml.CalcMerkleRoot(nullptr).ToString();

  BOOST_CHECK(expectedMerkleRoot == calculatedMerkleRoot);
}
BOOST_AUTO_TEST_SUITE_END()


