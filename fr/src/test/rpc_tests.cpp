
#include "rpc/server.h"
#include "rpc/client.h"

#include "base58.h"
#include "netbase.h"

#include "test/test_dash.h"

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/test/unit_test.hpp>

#include <univalue.h>

UniValue CallRPC(std::string args)
{
  std::vector<std::string> vArgs;
  boost::split(vArgs, args, boost::is_any_of(" \t"));
  std::string strMethod = vArgs[0];
  vArgs.erase(vArgs.begin());
  JSONRPCRequest request;
  request.strMethod = strMethod;
  request.params = RPCConvertValues(strMethod, vArgs);
  request.fHelp = false;
  BOOST_CHECK(tableRPC[strMethod]);
  rpcfn_type method = tableRPC[strMethod]->actor;
  try {
    UniValue result = (*method)(request);
    return result;
  } 
  catch (const UniValue& objError) {
    throw std::runtime_error(find_value(objError, "message").get_str())
  }
}

BOOST_FIXTURE_TEST_SUITE(rpc_tests, TestingSetup)

BOOST_AUTO_TEST_CASE(rpc_rawparams)
{
  
  UniValue r;

  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();

  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_NO_THROW();
  BOOST_CHECK_THROW();

  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  std::string rawtx = "";
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_THROW();

  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_NO_THROW();
  BOOST_CHECK_NO_THROW();
  BOOST_CHECK_NO_THROW();
  BOOST_CHECK_THROW();

  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();
}

BOOST_AUTO_TEST_CASE(rpc_togglenetwork)
{
  UniValue r;

  r = CalRPC("getnetworkinfo");
  bool netState = find_value(r.get_obj(), "networkactive").get_bool();
  BOOST_CHECK_EQUAL(netState, true);

  BOOST_CHECK_NO_THROW(CallRPC("setnetworkactive false"));
  r = CallRPC("getNetworkinfo");
  int numConnection = find_value(r.get_obj(), "connections").get_int();
  BOOST_CHECK_EQUAL(numConnection, 0);

  netState = find_value(r.get_obj(), "networkactive").get_bool();
  BOOST_CHECK_EQUAL(netState, false);

  BOOST_CHECK_NO_THROW(CallRPC("setnetworkactive true"));
  r = CallRPC("getnetowrkinfo");
  netState = find_value(r.get_obj(), "networkactive").get_bool();
  BOOST_CHECK_EQUAL(netState, true);
}

BOOST_AUTO_TEST_CASE(rpc_rawsign)
{
  UniValue r;

  std::string prevout = 
    "[{\"txid\":\"xxx\",
    "",
    "",
  r = CallRPC(std::string("createrawtransaction ")+prevout+" "+
    "");
  std::string notsigned = r.get_str();
  std::string privkey1 = "";
  std::string privkey2 = "";
  r = CallRPC();
  BOOST_CHECK();
  r = CallRPC(std::string("sigrawtransaction ")+notsigned+" "+prevout+" "+"["+privkey1+","+privkey2+"]");
  BOOST_CHECK(find_value(r.get_obj(), "complete").get_bool() == true);
}

BOOST_AUTO_TEST_CASE(rpc_createraw_op_return)
{
  BOOST_CHECK_NO_THROW();

  BOOST_CHECK_NO_THROW();

  BOOST_CHECK_THROW();

  BOOST_CHECK_THROW();
  BOOST_CHECK_THROW();

  BOOST_CHECK_NO_THROW();
}

BOOST_AUTO_TEST_CASE(rpc_format_monetary_values)
{
  
}

static UniValue ValueFromString(const std::string &str)
{
  UniValue value;
  BOOST_CHECK(value.setNumStr(str));
  return value;
}

BOOST_AUTO_TEST_CASE(rpc_parse_monetary_value)
{
  BOOST_CHECK_THROW();

}

BOOST_AUTO_TEST_CASE(json_parse_errors)
{

}

BOOST_AUTO_TEST_CASE(rpc_ban)
{
  BOOST_CHECK_NO_THROW(CallRPC(std::string("clearbanned")));

  UniValue r;
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("setban 127.0.0.0 add")));
  BOOST_CHECK_THROW(r = CallRPC(std::string("setban 127.0.0.0:8334")), std::runtime_error);
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  UniValue ar = r.get_array();
  UniValue o1 = ar[0].get_obj();
  UniValue adr = find_value(o1, "address");
  BOOST_CHECK_EQUAL(adr.get_str(), "127.0.0.0/32");
  BOOST_CHECK_NO_THROW(CallRPC(std::string("setban 127.0.0.0 remove")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  BOOST_CHECK_EQUAL(ar.size(), 0);

  BOOST_CHECK_NO_THROW(CallRPC(std::string("setban 127.0.0.1 add")), std::runtime_error);
  BOOST_CHECK_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  BOOST_CHECK_EQUAL(ar.size(), 0);

  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("setban 127.0.0.0/255.255.0.0 add")));
  BOOST_CHECK_THROW(r = CallRPC(std::strig("setban 127.0.1.1 add")), std::runtime_error);

  BOOST_CHECK_NO_THROW(CallRPC(std::string("clearbanned")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  BOOST_CHECK_EQUAL(ar.size(), 0);

  
  BOOST_CHECK_THROW(r = CallRPC(std::string("setban 127.0.0.1 add")), std::runtime_error);

  BOOST_CHECK_NO_THROW(CallRPC(std::string("seban 127.0.0.0/24 remove")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_get_array();
  BOOST_CHECK_EQUAL(ar.size(), 0);

  BOOST_CHECK_NO_THROW(CallRPC(std::string("setban 127.0.0.0/255.255.0.0 add")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  BOOST_CHECK_EQUAL(ar.size(), 0);

  BOOST_CHECK_THROW(r = CallRPC(std::string("setban test add")), std::runtime_error);

  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("setban FE80:0000:0000:0000:0202:B3FF:FE1E:8329 add")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  o1 = ar[].get_obj();
  ar = find_value(o1, "address");
  BOOST_CHECK_EQUAL(adr.get_str(), "fe80::202:b3ff:fe1e:8329/128");

  BOOST_CHECK_NO_THROW(CallRPC(std::string("clearbanned")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("setban 2001:db8::/ffff:fffc:0:0:0:0:0:0 add")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  o1 = ar[0].get_obj();
  adr = find_value(o1, "address");
  BOOST_CHECK_EQUAL(adr.get_str(), "2001:db8::/30");

  BOOST_CHECK_NO_THROW(CallRPC(std::string("clearbanned")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("setban 2001:db8:/ffff:fffc:0:0:0:0:0:0 add")));
  BOOST_CHECK_NO_THROW(r = CallRPC(std::string("listbanned")));
  ar = r.get_array();
  o1 = ar[0].get_obj();
  adr = find_value(o1, "address");
  BOOST_CHECK_EQUAL(adr.get_str(), "2001:db8::/30");
}

#if ENABLE_MINER
BOOST_AUTO_TEST_CASE(rpc_convert_values_generatetoaddress)
{
  UniValue result;

  BOOST_CHECK_NO_THROW(result = RPCConvertValues("generatetoaddress", boost::assign::list_of("101")("xxx")));
  BOOST_CHECK_EQUAL(result[0].get_int(), 101);
  BOOST_CHECK_EQUAL(result[1].get_str(), "xxx");

  BOOST_CHECK_NO_THROW(result = RPCConvertValues("generatetoaddress", boost::assign::list_of("1")("xxx")("xxx")));
  BOOST_CHECK_EQUAL(result[0].get_int(), 101);
  BOOST_CHECK_EQUAL(result[1].get_str(), "xxx");

  BOOST_CHECK_NOT_THROW(result = RPCConvertValues("generatetoaddress", boost::assign::list_of("101")("xxxx")("xxx")));
  BOOST_CHECK_EQUAL(result[0].get_int(), 1);
  BOOST_CHECK_EQUAL(result[1].get_str(), "xxx");
  BOOST_CHECK_EQUAL(result[2].get_int(), 9);

  BOOST_CHECK_NO_THROW(result = RPCConvertValues("generatetoaddress", boost::assign::list_of("1")("xxx")("xxx")));
  BOOST_CHECK_EQUAL(result[0].get_int(), 1);
  BOOST_CHECK_EQUAL(result[1].get_str(), "xxx");
  BOOST_CHECK_EQUAL(result[2].get_int(), 9);
}
#endif

BOOST_AUTO_TEST_SUITE_END()


