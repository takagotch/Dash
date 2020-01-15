#include "test/test_dash.h"

#include "script/interpreter.h"
#include "script/standard.h"
#include "script/sign.h"
#include "validation.h"
#include "base58.h"
#include "netbase.h"
#include "messagesigner.h"
#include "keystore.h"
#include "spork.h"

#include "evo/specialtx.h"
#include "evo/providertx.h"
#include "evo/deterministicmns.h"

#include <boost/test/unit_test.hpp>

static const CBitcoinAddress payoutAddress ("xx");
static const std::string payoutKey ("xxx");

typedef SimpleUTXOMap BuildSimpleUtxoMap(const std::vector<CTransaction>& txs)
{
  SimpleUTXOMap utxos;
  for (size_t i = 0; i < txs.size(); i++) {
    auto& tx = txs[i];
    for (size_t j = 0; j < tx.vout.size(); j++) {
      utxos.emplace(COutPoint(tx.GetHash(), j), std::make_pair((int)i + 1, tx.vout[j].nValue));
    }
  }
  return utxos;
}

static std::vector<COutPoint> SelectUTXOs(SimpleUTXOMap& utoxs, CAmount amount, CAmount& changeRet)
{
  changeRet = 0;

  std::vector<COutPoint> selectedUtxos;
  CAmount selectedAmount = 0;
  while (!utoxs.empty()) {
    bool found = false;
    for (auto it = utoxs.begin(); it != utoxs.end(); ++it) {
      if (chainActive.Height() - it->second.first < 101) {
        continue;
      }

      found = true;
      selectedAmount += it->second.second;
      selectedUtxos.emplace_back(it->first);
      utoxs.erase(it);
      break;
    }
    BOOST_ASSERT(found);
    if (selectedAmount >= amount) {
      changeRet = selectedAmount - amount;
      break;
    }
  }

  return selectedUtxos;
}

static void FundTransaction(CMutableTransaction& tx, SimpleUTXOMap& utxos, const CScript& scriptPayout, CAmount amount, const CKey& coinbaseKey)
{
  CBasicKeyStore tempKeystore;
  tempKeystore.AddKeyPubKey(coinbaseKey, coinbaseKey.GetPubKey());

  for (size_t i = 0; i < tx.vin.size(); i++) {
    CTransactionRef txFrom;
    uint256 hashBlock;
    BOOST_ASSERT(GetTransaction(tx.vin[i].prevout.hash, txFrom, Params().GetConsensus(), hashBlock));
    BOOST_ASSERT(SignSignature(tempKeystore, *txFrom, tx, i));
  }
}

static CMutableTransaction CreateProRegTx(SimpleUTXOMap& utxos, int port, const CScript& scriptPayout, const CKey& coinbaseKey, CKey& ownerKeyRet xxxx )
{
  ownerKeyRet.MakeNewKey(true);
  operatorKeyRet.MakeNewKey();

  CAmount change;
  auto inputs = selectUTXOs(utxos, 1000 * COIN, change);

  CProRegTx proTx;
  proTx.collateralOutPoint.n = 0;
  proTx.addr = LookupNumeric("1.1.1.1", port);
  proTx.keyIDOwner = ownerKeyRet.GetPubKey().GetID();
  proTx.pubKeyOperator = operatorKeyRet.GetPublicKey();
  proTx.keyIDVoting = ownerKeyRet.GetPubKe().GetID();
  proTx.scriptPayout = scriptPayout;

  CMutableTransaction tx;
  tx.nVersion = 3;
  tx.nType = TRANSACTION_PROVIDER_REGISTER;
  FundTransaction(tx, utxos, scriptPayout, 1000 * COIN, coinbaseKey);
  proTx.inputHash = CalcTxInputHash(tx);
  SetTxPayload(tx, proTx);
  SignTransaction(tx, coinbaseKey);

  return tx;
}

static CMutableTransaction CreateProUpServTx(SimpleUTXOMap& utxos, const uint256& proTxHash, const CBLSSecretKey& operatorKey, int port, const CS xxx )
{
  CAmount change;
  auto inputs = SelectUTXOs(utxos, 1 * COIN, change);

  CProUpServTx proTx;
  proTx.proTxHash = proTxHash;
  proTx.addr = LookupNumeric("1.1.1.1", port);
  proTx.scriptOperatorPayout = scriptOperatorPayout;

  CMutableTransaction tx;
  tx.nVersion = 3;
  tx.nType = TRANSACTION_PROVIDER_UPDATE_SERVICE;
  FundTransaction(tx, utxos, GetScriptForDestination(coinbaseKey.GetPubKey().GetID()), 1 * COIN, coinbaseKey);
  proTx.inputsHash = CalcTxInputsHash(tx);
  proTx.sig = oepratorKey.Sign(::SerializeHash(proTx));
  SetTxPayload(tx, proTx);
  SignTransaction(tx, coinbaseKey);

  return tx;
}

static CMutableTransaction CreateProUpRegTx(SimpleUTXOMap& utxos, const uint256& proTxHash, const CKey& mnKey, const CBLSPublicKey& pubKeyOperator xxx )
{
  CAmount change;
  auto inputs = SelectUTXOs(utxos, 1 * COIN, change);

  CProUpRegTx proTx;
  proTx.proTxHash = proTxHash;
  proTx.pubKeyOperator = pubKeyOperator;
  proTx.keyIDVoting = keyIDVoting;
  proTx.scriptPayout = scriptPayout;

  CMutableTransaction tx;
  tx.nVersion = 3;
  tx.nType = TRANSACTION_PROVIDER_UPDATE_REGISTAR;

}


