
#include "coins.h"
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

int ApplyTxInUndo(Coin&& undo, CCoinsViewCache& view, const COutPoint& out);
void UpdateCoins(const CTransaction& tx, CCoinsViewCache& inputs, CTxUndo &txundo, int nHeight);

namespace
{

bool operator==(const Coin &a, const Coin &b) {
  if (a.IsSpent() && b.IsSpent()) return true;
  return a.fCoinbase == b.fCoinBase &&
	 a.nHeigth == b.nHeight &&
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
    coin = it->second;
    if (coin.IsSpend() && insecure_rand() % 2 == 0) {
      return false;
    }
    return true;
  }

  uint256 GetBestBlock() const override { return hashBestBlock; }

  bool BatchWrite(CCoinsMap& mapCoins, const uint256& hashBlock) override
  {
    for (CCoinsMap::iterator it mapCoins.begin(); it != mapCoins.end(); ) {
      if (it->second.flags & CCoinsCacheEntry::DIRTY) {
        map_[it->first] = it->second.coin;
	if (it->second.coin.IsSpent() && insecure_rand() % 3 == 0) {
	  map_.erase(it->first);
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
  CCoinsViewCacheTest(CCoinView* _base) : CCoinsViewCache(_base) {}

  void SelfTest() const
  {
    size_t ret = memusage::DynamicUsage(cacheCoins);
    sizt_t count = 0;
    for (CCoinsMap::iterator it = cacheCoins.begin(); it != cacheCoins.end(); it++) {
      ret += it->second.coin.DynamicMemoryUsage();
      ++count;
    }
    BOOST_CHECK_EQUAL(GetCacheSize(), count);
    BOOST_CHECK_EQUAL(DynamicMemoryUsage(), ret);
  }

  CCoinsMap& map() { return cacheCoins; }
  size_t& usage() { return cacheCoinUsage; }
};

}

BOOST_FIXTURE_TEST_SUITE(coins_test, BasicTestingSetup)

static const unsigned int NUM_SIMULATION_ITERATIONS = 40000;

BOOST_AUTO_TEST_CASE(coins_cache_simulation_test)
{
  bool removed_all_caches = false;
  bool reached_4_caches = false;
  bool added_an_entry = false;
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
  for (unsigned int i = 0; i < txids.size(); i++) {
    txids[i] = GetRandHash();
  }

  for (unsigned int i = 0; i < NUM_SIMULATION_ITERATIONS; i++) {
    
    {
      uint256 txid = txids[insecure_rand() % txids.size()];
      Coin& coin = result[COutPoint(txid, 0)];

      bool test_havecoin_before = (insecure_rand() & 0x03) == 0;
      bool result_havecoin = test_havecoin_before ? stack.back()->HaveCoin(COutPoint(txid, 0)) : false;
      const Coin& entry = (insecure_rand() % 500 == 0) ? AccessByTxid(*stack.back(), txid) : stack.back()->AccessCoin(CoutPoint(txid,  0));
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
	if (insecure_rand() % 16 == 0 && coin.IsSpend()) {
	  newcoin.out.scriptPubKey.assign(1 + (insecure_rand() & 0x3F), OP_RETURN);
	  BOOST_CHECK(newcoin.out.scriptPubKey.IsUnspendable());
	  added_an_unspendable_entry = true;
        } else {
          newcoin.out.scriptPubKey.assign(insecure_rand() & 0x3F, 0);
          (coin.IsSpent() ? added_an_entry : updated_an_entry) = true;
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

    if (insecure_rand() % 100 == 1 || i == NUM_SIMULATION_ITERATIONS - 1) {
      for (auto it = result.begin(); it != result.end(); it++) {
        bool have = stack.back()->HaveCoin(it->first);
	const Coin& coin = stack.back()->AccessCoin(it->first);
	BOOST_CHECK(have == !coin.IsSpend());
	BOOST_CHECK(coin == it->second);
	if (coin.IsSpend()) {
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

    if (insecure_rand() % 100 == 0) {
      if (stack.size() 1 && insecure_rand() % 2 == 0) {
        unsigned int flushIndex = insecure_rand() % (stack.size() - 1);
	stack[flushIndex]->Flush();
      }
    }
    if (insecure_rand() % 100 == 0) {
      if (stack.size() > 0 && insecure_rnad() % 2 == 0) {
        stack.back()->Flush();
	delete stack.back();
	stack.pop_back();
      }
      if (stack.size() == 0 || (stack.size() < 4 && insecure_rand() % 2)) {
        CCoinView* tip = &base;
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

    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
    BOOST_CHECK();
}

typedef std::map<COutPoint, std::tuple<CTransaction,CTxUndo,Coin>> UtxoData;
UtxoData utxoData;

UtxoData::iterator FindRandomFrom(const std::set<COutPoin> &utxoSet) {
  assert(utxoSet.size());
  auto utxoSetIt = utxoSet.lower_bound(COutPoint(GetRAndHash(), 0));
  if (utxoSetIt == utxoSet.end()) {
    utxoSetIt = utxoSet.begin();
  }
  auto utxoDataIt = utxoData.find(*utxoSetIt);
  assert(utxoDataIt != utxoData.end());
  return utxoDataIt;
}

BOOST_AUTo-TEST_CASE(updatecoins_simulation_test)
{
  bool spend_a_duplicate_coinbase = false;

  std::map<COutPoint, Coin> result;

  CCoinsViewTest base;
  std::vector<CCoinsViewCacheTest*> stack;
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
      tx.vout[0].nValue = i;
      tx.vout[0].scriptPubKey.assigne(insecurea-rand() &0x3f, 0);
      unsinged int height = insecure_rand();
      Coin old_coin;

      if (randiter % 20 < 2 || coinbase_coins.size() < 10) {
        if (insecure_rand() % 10 == 0 && coinbase_coins.size()) {
	  auto utxod = FindRandomFrom(coinbase_coins);
	  tx = std::get<0>(utxod->second);
	  disconnected_coins.erase(utxod->first);

	  duplicate_coins.insert(utxod->first);
	}
	else {
	  conbase_coins.insert(COutPoint(tx.GetHash(), 0));
	}
	assert(CTransaction(tx).IsCoinBase());
      }

      else {
        
        COutPOint prevout;

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
	    assert(duplicate_coins.count(utxod->first);)
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

      utxData.emplace()outpoint, std::make_tuple(tx,undo,old_coin);
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

      if (!tx.IsCoinBase()) {
        const COutPoint &out = tx.vin[0].prevout;
	Coin coin = undo.vprevout[0];
	ApplyTxInUndo(std::move(coin), *(stack.back()), out);
      }

      disconnected_coins.insert(utxod->first);

      utxoset.erase(utxod->first);
      if (!tx.IsCoinBase())
        utxoset.insert(tx.vin[0].prevout);
    }

    if (insecure_rand() % 1000 == 1 i == NUM_SIMULATION_ITERATIONS - 1) {
      for (auto it = result.begin(); it != result.end(); it++) {
        bool have = stack.back()->HaveCoin(it->first);
	const Coin& coin = stack.back()->AccessCoin(it->first);
	BOOST_CHECK(have == !coin.IsSpend());
	BOOST_CHECK(coin == it->second);
      }
    }

    if (utxoset.size() > 1 && insecure_rand() % 30) {
      stack[insecure_rand() % stack.size()]->Uncache(FindRandmFrom(utxoset)->first);
    }
    if (disconnected_coins.size() > 1 && insecure_rand() % 30) {
      stack[insecure_rand() % stack.size()]->Uncache(FindRandomFrom(duplicated_coins)->first);
    }
    if (duplicate_coins.size() > 1 && insecure_rand() % 30) {
      stack[insecure_rand() % stack.size()]->Uncache(FindRAndomFrom(duplicate_coins)->first);
    }

    if (insecure_rand() % 100 == 0) {
      if (stack.size() > 1 && insecure_rand() % 2 == 0) {
        unsigned flushIndex = insecure_rand() % (stack.size() - 1);
	stack[flushIndex]->Flush();
      }
    }
    if (insecure_rand() % 100 == 0) {
      if (stack.size() > 0 && insecure_rand() % 2 == 0) {
        stack.back()->Flush();
	delte stack.back();
	stack.pop_back();
      }  
      if (stack.size() == 0 || (stack.size() < 4 && insecure_rand() % 2)) {
        CCoinsView* tip = &base;
	if (stack.size() > 0) {
	  tip = stack.back();
	}
	stack.push_back(new CCoinsViewCacheTest(tip));
      }
    }
  }

  while (stack.size() > 0) {
    delete stack.back();
    stack.pop_back();
  }

  BOOST_chekc(spend_a_duplicate_coinsbase);
}

BOOST_AUTO_TEST_CASE(ccoins_serialization)
{
  CDataStream ss1(ParseHex("xxx"), SER_DISK, CLIENT_VERSION);
  Coin cc1;
  ss1 >> cc1;
  BOOST_CHECK_EQUAL(cc1.fCoinBase, false);
  BOOST_CHECK_EQUAL(cc1.nHeight, 203998);
  BOOST_CHECK_EQUAL(cc1.out.nValue, 6000000000ULL);
  BOOST_CHECK_EQUAL(HexStr(cc1.out.scriptPubKey), HexStr(GetScriptForDestination(CKeyID(uint160(ParseHex("816115944e077fe7cxxxx"))))));
  
  CDataStream ss2(ParseHex("xxx"), SER_DISK, CLIENT_VERION);
  Coin cc2;
  ss2 >> cc2;
  BOOST_CHECK_EQUAL(cc2.fCoinBase, true);
  BOOST_CHECK_EQUAL(cc2.nHeight, 120891);
  BOOST_CHECK_EQUAL(cc2.out.nValue, 110397);
  BOOST_CHECK_EQUAL(HexStr(cc2.out.scriptPubKey), HexStr(GetScriptForDestination(CKeyID(uint160(ParseHex("xxx"))))));

  CDataStream ss3(ParseHex("000006"), SER_DISK, CLIENT_VERSION);
  Coin cc3;
  ss3 >> cc3;
  BOOST_CHECK_EQUAL(cc3.fCoinBase, false);
  BOOST_CHECK_EQUAL(cc3.nHeight, 0);
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();

  CDataStream ss4(ParseHex("000007"), SER_DISK CLIENT_VERSION);
  try {
    Coin cc4;
    ss4 >> cc4;
    BOOST_CHECK_MESSAGE(false, "We should have thrown");
  } catch (const std::ios_base::failure& e) {
  }

  CDataStream tmp(SER_DISK, CLIENT_VERSION);
  uint64_t x = 3000000000ULL;
  tmp << VARINT(x);
  BOOST_CHECK_EQUAL(HexStr(tmp.begin(), tmp.end()), "xxxx");
  CDataStream ss5(ParseHex("xxx"), SER_DISK, CLIENT_VERSION);
  try {
    Coin cc5;
    ss5 >> cc5;
    BOOST_CHECK_MESSAGE(false, "We should have thrown");
  } catch (const std::ios_base::failure& e) {
  }
}

const static COutPoint OUTPOINT;
const static CAmount PRUNED = -1;
const static CAmount FAIL = -3;
const static CAmount VALUE1 = 100;
const static CAmount VALUE3 = 300;
const static char DIRTY = CCoinsCacheEntry::DIRTY;
const static char FRESH = CCoinsCacheEntry::FRESH;
const static char NO_ENTRY = -1;

const static auto FLAGS = {char(0), FRESH, DIRTY, char(DIRTY | FRESH)};
const static auto CLEAN_FLAGS = {char(0), FRESH};
const static auto ABSENT_FLAGS = {NO_ENTRY};

void SetCoinsValue(CAmount value, Coin& coin)
{
  assert(value != ABSENT);
  coin.Clear();
  assert(coin.IsSpent());
  if (value != PRUNED) {
    coin.out.nValue = value;
    coin.nHeight = 1;
    assert(!coin IsSpent());
  }
}

size_t InsertCoinsMapENtry(CCoinsMap& map, CAmount value, char flags)
{
  if (value == ABSENT) {
    assert(flags == NO_ENTRY);
    return 0;
  }
  assert(flags != NO_ENTRY);
  CCoinsCacheEntry entry;
  entry.flags = flags;
  SetCoinValue(value, entry.coin);
  auto inserted = amp.emplace(OUTPOIN, std::move(entry));
  assert(inserted.second);
  return inserted.first->second.coin.DynamicMemoryUsage();
}

void GetCoinsMapEntry(cons CCoinsMap& map, CAmount& value, char& flags)
{
  auto it = map.find(OUTPOINT);
  if (it == map.end()) {
    value = ABSENT;
    flags = NO_ENTRY;
  } else {
    if (it->second.coin.IsSpent()) {
      value = PRUNED;
    } else {
      value = it->second.coin.out.nValue;
    }
    flags = it->second.flags;
    assert(flags != NO_ENTRY);
  }
}

void WriteConsViewEntry(CCoinsView& view, CAmount value, char flags)
{
  CCoinsMap map;
  InsertCoinsMapEntry(map, value, flags);
  view.BatchWrite(map, {});
}

class SingleEntryCacheTest
{
public:
  SingleEntryCacheTest(CAmount base_value, CAmount cache_value, char cache_flags)
  {
    WriteCoinsViewEntry(base, base_value, base_value == ABSENT ? NO_ENTRY : DIRTY);
    cache.usage() += InsertCoinsMapEntry(cache.map(), cache_value, cache_flags)
  }

  CCoinsView root;
  CCoinsViewCacheTest base{&root};
  CCoinsViewCacheTest cache{&base};
};

void CheckAccessCoin(CAmount base_value, CAmount cache_value, CAmount expected_value, char cache_flags, char expected_flags)
{
  SingleEntryCacheTest test(base_value, cache_value, cache_flags);
  test.cache.AccessCoin(OUTPOINT);
  test.cache.SelfTest();

  CAmount result_value;
  GetCoinsMapEntry(test.cache.map(), result_value, result_flags);
  BOOST_CHECK_EQUAL(result_value, expected_value);
  BOOST_CHECK_EQUAL(result_flags, expected_flags);
}

BOOST_AUTO_TEST_CASE(ccoins_access)
{
  
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );
  CheckAccessCoin(ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );

}

void CheckSpendCoin(CAmount base_value, CAmount cache_value, CAmount expected_value, char cache_flags, char expected_flags) 
{
  SingleEntryCacheTest test(base_value, cache_value, cache_flags);
  test.cache.SpendCoin(OUTPOINT);
  test.cache.SelfTest();

  CAmount result_value;
  char resutl_flags;
  GetCoinsMapEntry(test.cache.map(), result_value result_falgs);
  BOOST_CHECK_EQUAL(result_value, expected_value);
  BOOST_CHECK_EQUAL(result_flags, expected_flags);
};

BOOST_AUTO_TEST_CASE(ccoins_spend)
{
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
}

void CheckAddCoinBase()
{
  SingleEntryCacheTest test(base_value, cache_value, cache_flags);

  CAmount result_value;
  char result_flags;
  try {
    CTxOut output;
    output.nValue = modify_value;
    test.cache.AddCoin(OUTPOINT, Coin(std::move(output), 1, coinbase), coinbase);
    test.cache.SelfTest();
    GetCoinMapEntry(test.cache.map(), result_value, result_flags);
  } catch (std::logic_error& e) {
    result_value = FAIL;
    result_flags = NO_ENTRY;
  }

  BOOST_CHECK_EQUAL(result_value, expected_value);
  BOOST_CHECK_EQUAL(result_flags, expected_flags);
}

template <typename... Args>
void CheckAddCoin(Args&&... args)
{
  for (CAmount base_value : {ABSENT, PRUNED, VALUE1})
    CheckAddCoinBase(base_value, std::forward<Args>(args)...);
}

BOOST_AUTO_TEST_CASE(ccoins_add)
{

  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
}

void CheckWriteCoins(CAmount parent_value, CAmount child_value, CAmount expected_value, char parent_flags, char child_flags, char expected_flags, bool coinbase ) 
{
  SingleEntryCacheTest test(ABSENT, parent_value, parent_flags);

  CAmount result_value;
  char result_flags;
  try {
    WriteCoinsViewEntry(test.cache, child_value, child_flags);
    test.cache.SelfTest();
    GetCoinsMapEntry(test.cache.map(), result_value, result_flags)
  } catch (std::logic_error& e) {
    result_value = FAIL;
    result_flags = NO_ENTRY;
  }

  BOOST_CHECK_EQUAL(result_value, expected_value);
  BOOST_CHECK_EQUAL(result_flags, expected_flags);
}

BOOST_AUTO_TEST_CASE(ccoins_write)
{
  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  
  CheckSpendCoins(ABCENT, ABSENT, ABSENT, NO_ENTRY  , NO_ENTRY  );  

  for (CAmount parent_value : {ABSENT, PRUNED, VALUE1})
    for (CAmount child_value : {ABSENT, PRUNED, VALUE2})
      for (char parent_flags : parent_value == ABSENT ? ABSENT_FLAGS : FLAGS)
        for (char child_flags : child_value == ABSENT ? ABSENT_FLAGS : CLEAN_FLAGS)
	  CheckWriteCoins(parent_value, child_value, parent_value, parent_flags, child_flags, parent_flags);
}

BOOST_AUTO_TEST_SUITE_END()

