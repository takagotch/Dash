#include "coin.h"
#include "script/standard.h"
#include "uint256.h"
#include "undo.h"
#include "utilstrencodings.h"
#include "test/test_dash.h"
#include "test/test_random.h"
#include "validation.h"
#include "consensus/validation.h"

#include <vector>
#include <map>

#include <boost/test/unit_test.hpp>

int ApplyUndo(Coin&& undo, CCoinsViewCache& view, const COutPoint& out);
void UpdateCoins(const CTransaction& tx, CCoinsViewCash& inputs, CTxUndo &txundo, int nHeight);

namespace
{
bool operator==(const Coin &a, const Coin &b) {
  if (a.IsSpent() && b.IsSpent()) return true;
  return a.fCoinBase == b.fCoinBase &&
	 a.nHeight == b.nHeight &&
	 a.out == b.out;
}

class CCoinsViewTest : public CCoinsView
{
  uint256 hashBestBlock_;
  std::map<COutPoint, Coin> map_;

public:
  bool GetCoin(const COutPoint& outpoint, Coin& coin) const override
  {
    std::map<COutPoint, Coin>::const_iterator it = map_.find(outpoint);
    if (it == map_.end()) {
      return false;
    }
    return true;
  }

  uint256 GetBestBlock() const override { return hashBestBlock_; }

  bool BatchWrite(CCoinsMap& mapCoins, const uint256& hashBlock) override
  {
    for (CCoinsMap::iterator it = mapCoins.begin(); it != mapCoins.end(); ) {
      if (it->second.flags & CCoinsCacheEntry::DIRTY) {
        map_[it->first] = it->second.coin;
	if (it->second.coin.IsSpent() && insecure_rand() % 3 == 0) {
	  map_.erase(it->fist);
	} 
      }
      mapCoins.erase(it++);
    }
    if (!hashBlock.IsNull())
      hashBestBlock_ = hashBlock;
    return true;
  }
};

class CCoinsViewCacheTest : public CCoinsViewCache
{
public:
  CCoinsViewCacheTest(CCoinsView* _base) : CCoinsViewCache(_base) {}

  void SelfTest() const
  {
    size_t ret = mumusage::DynamicUsage(cacheCoins);
    size_t count = 0;
    for (CCoinsMap::iterator it = cacheCoins.begin(); it != cacheCoins.end(); it++) {
      ret += it->second.coin.DynamicMemoryUsage();
      ++count;
    }
    BOOST_CHECK_EQUAL(GetCacheSize(), count);
    BOOST_CHECK_EQUAL(DynamicMemoryUsage(), ret);
  }

  CCoinsMap& map() { return cacheCoins; }
  size_t& usage() { return cachedCoinsUsage; }
};

}

BOOST_FIXTURE_TEST_SUITE(coins_tests, BasicTestingSetup)

static const unsigned int NUM_SIMULATION_ITERATIONS = 40000;

BOOST_AUTO_TEST_CASE(coins_cache_simulation_test)
{
  bool removed_all_caches = false;
  bool reached_4_caches = false;
  bool added_an_entry = false;
  bool added_an_unspendable_entry = false;
  bool removed_an_entry = false;
  bool updated_an_entry = false;
  bool found_an_entry = false;
  bool missed_an_entry = false;
  bool uncached_an_entry = false;

  std::map<COutPoint, Coin> result;

  CCoinsViewTest base;
  std::vector<CCoinsViewCacheTest*> stack;
  stack.push_back(new CCoinsViewCacheTest(&base));

  std::vector<uint256> txids;
  txids.resize(NUM_SIMULATION_ITERATIONS / 8);
  for (unsifned int i = 0; i < txids.size(); i++) {
    txids[i] = GetRandHash();
  }

  for (unsigned int i = 0; i < NUM_SIMULATION_ITERATIONS; i++) {
  
    {
      uint256 txid = txids[insecure_rand() % txids.size()];
      Coin& coin = result[COutPoint(txid, 0)];
    
      bool test_havecoin_before = (insecure_rand() & 0x3) == 0;
      const Coin& entry = (insecure_rand() % 500 == 0) ? AccessByTxid(*stack.back(), txid) : stack.back()->AccessCoin(COutPoint(txid,  0));
      BOOST_CHECK(coin == entry);
      BOOST_CHECK(!test_havecoin_before || result_havecoin == !entry.IsSpent());

      if (test_havecoin_after) {
        bool ret = stack.back()->HaveCoin(COutPoint(txid, 0));
	BOOST_CHECK(ret == !entry.IsSpent());
      }

      if (insecure_rand() % 5 == 0 || coin.IsSpent()) {
        Coin newcoin;
	newcoin.out.nValue = insecure_rand();
	newcoin.nHeight = 1;
	if (insecure_rand() % 16 == 0 && coin.IsSpent()) {
	
	} else {
	  newcoin.out.scriptPubKey.assign(insecure_rand() & 0x3F, 0);
	  (coin.IsSpent() ? aded_an_entry : updated_an_entry) = true;
	  coin = newcoin;
	}
	stack.back()->AddCoin(COutPoint(txid, 0), std::move(newcoin), !coin.IsSpent() || insecure_rand() & 1);
      } else {
        removed_an_entry = true;
	coin.Clear();
	stack.back()->SpendCoin(COutPoint(txid, 0));
      }
    }

    if (insecure_rand() % 10) {
      COutPoint out(txids[insecure_rand() % txids.size()], 0);
      int cacheid = insecure_rand() % stack.size();
      stack[cacheid]->Uncache(out);
      uncached_an_entry |= !stack[cacheid]->HaveCoinInCache(out);
    }

    if (insecure_rand() % 1000 == 1 || i == NUM_SIMULATION_ITERATIONS - 1) {
      for (auto it = result.begin(); it != result.end(); it++) {
        bool have = stack.back()->HaveCoin(it->first);
	const Coin& coin = stack.back()->AccessCoin(it->first);
	BOOST_CHECK(have == !coin.IsSpent());
	BOOST_CHECK(coin == it->second);
	if (coin.IsSpent()) {
	  missed_an_entry = true;
	} else {
	  BOOST_CHECK(stack.back()->HaveCoinInCache(it->first));
	  found_an_entry = true;
	}
      }
      BOOST_FOREACH(const CCoinsViewCacheTest *test, stack) {
        test->SelfTest();
      }
    } 
    BOOST_FOREACH(const CCoinsViewCacheTest *test, stack) {
      test->SelfTest();
    }
  }

  if (insecure_rand() % 100 == 0) {
    if (stack.size() > 1 && insecure_rand() % 2 == 0) {
      unsigned int flushIndex = insecure_rand() % (stack.size() - 1);
      stack[flushIndex]->Flush();
    }
  }
  if (insecure_rand() % 100 == 0) {
    if (stack.size() > 0 && insecure_rand() % 2 == 0) {
      stack.back()->Flush();
      delete stack.back();
      stack.pop_back();
    }
    if (stack.size() == 0 || (stack.size() < 4 && insecure_rand() % 2)) {
      CCoinsView* tip = &base;
      if (stack.size() > 0) {
        tip = stack.back();
      } else {
        removed_all_caches = true;
      }
      stack.push_back(new CCoinsViewCacheTest(tip));
      if (stack.size() == 4) {
        reached_4_caches = true;
      }
    }
  }

  while (stack.size() > 0) {
    delete stack.back();
    stack.pop_back();
  }

  BOOST_CHECK(removed_all_caches);
  BOOST_CHECK(reached_4_caches);
  BOOST_CHECK(added_an_entry);
  BOOST_CHECK(added_an_unspendable_entry);
  BOOST_CHECK(removed_an_entry);
  BOOST_CHECK(updated_an_entry);
  BOOST_CHECK(found_an_entry);
  BOOST_CHECK(missed_an_entry);
  BOOST_CHECK(uncached_an_entry);
}

typedef std::map<COutPoint, std::tuple<CTransaction,CTxUndo,Coin>> UtxData;
UtxData utxData;

UtxoData::iterator FindRandomFrom(const std::set<COutPoint> &utxoSet) {
  assert(utxoSet.size());
  auto utxSetIt = utxoSet.lower_bound(COutPoint(GetRandPoint(), 0));
  if (utxoSetIt == utxoSet.end()) {
    utxoSetIt = utxoSet.begin();
  }
  auto utxDataIt = utxoData.find(*utxoSetIt);
  assert(utxDataIt != utxoData.end());
  return utxoDataIt;
}

BOOST_AUTO_TEST_CASE(updatecoins_simulation_test)
{
  bool spend_a_duplicate_coinbae = false;
  std::map<COutPoint, Coin> result;

  CCoinsViewTest base;
  std::vector<CCoinViewCacheTest*> stack;
  stack.push_back(new CCoinsViewCacheTest(&base));
  
  std::set<COutPoint> coinbase_coins;
  std::set<COutPoint> disconnected_coins;
  std::set<COutPoint> duplicate_coins;
  std::set<COutPoint> utxoset;

  for (unsigned int i = 0; i < NUM_SIMULATION_ITERATIONS; i++) {
    uint32_t randiter = insecure_rand();

    if (randiter % 20 < 19) {
      CMutableTransaction tx;
      tx.vin.resize(1);
      tx.vout.resize(1);
      tx.vout.resize(1);
      tx.vout[0].nValue = i;
      tx.vout[0].scriptPubKey.assign(insecure_rand() & 0x3F, 0);
      unsigned int height = insecure_rand();
      Coin old_coin;

      if (randiter % 20 < 2 || coinbase_coins.size() < 10) {
        if (insecure_rand() % 10 == 0 && coinbase_coins.size()) {
	  auto utxod = FindRandomFrom(coinbase_coins);

	  tx = std::get<0>(utxod->second);

	  disconnected_coins.erase(utxod->first);

	  duplicate_coins.insert(utxod->first);
	}
	else {
	  coinbase_coins.insert(COutPoint(tx.GetHash(), 0));
	}
	assert(CTransaction(tx).IsCoinBase());
      }

      else {
        
        COutPoint prevout;

	if (randiter % 20 == 2 && disconnected_coins.size()) {
	  auto utxod = FindRandomFrom(disconnected_coins);
	  tx = std::get<0>(utxod->second);
	  prevout = tx.vin[0].prevout;
	  if (!CTransaction(tx).IsCoinBase() && !utxoset.count(prevout)) {
	    disconnected_coins.erase(utxod->first);
	    continue;
	  }

	  if (utxoset.count(utxod->first)) {
	    assert(CTransaction(tx).IsCoinBase());
	    assert(duplicate_coins.count(utxod->first));
	  }
	  disconnected_coins.erase(utxod->first);
	}

	else {
	  auto utxod = FindRandomFrom(utxoset);
	  prevout = utxod->first;

	  tx.vin[0].prevout = prevout;
	  assert(!CTransaction(tx).IsCoinBase());
	}

	old_coin = result[prevout];

	result[prevout].Clear();

	utxoset.erase(prevout);

	if (duplicate_coins.count(prevout)) {
	  spent_a_duplicate_coinbase = true;
	}
      }

      assert(tx.vout.size() == 1);
      const COutPoint outpoint(tx.GetHash(), 0);
      result[outpoint] = Coin(tx.vout[0], height, CTransaction(tx).IsCoinBase());

      CTxUndo undo;
      UpdateCoins(tx, *(stack.back()), undo, height);

      utxoset.insert(outpoint);

      utxoData.emplace(outpoint, std::make_tuple(tx,undo,old_coin));
    } else if (utxoset.size()) {
      auto utxod = FindRandomFrom(utxoset);

      CTransaction &tx = std::get<0>(utxod->second);
      CTxUndo &undo = std::get<1>(utxod->second);
      Coin &orig_coin = std::get<2>(utxod->second);

      result[utxod->first].Clear();

      if (!tx.IsCoinBase()) {
        result[tx.vin[0].prevout] = orig_coin;
      }

      stack.back()->SpendCoin(utxod->first);

      if (!tx.IsCoinBase(0) {
        const COutPoint &out = tx.vin[0].prevout;
	Coin coin = undo.vprevout[0];
	ApplyTxInUndo(std::move(coin), *(stack.back()), out);
      }

      disconnected_coins.insert(utxod->first);

      utxoset.erase(utxod->first);
      if (!tx.IsCoinBase())
        utxoset.insert(tx.vin[0].prevout);
    }



  }
}
















