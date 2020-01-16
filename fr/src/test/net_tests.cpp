
#include "addrman.h"
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""
#include ""

class CAddrManSerializationMock : public CAddrMan
{
public:
  virtual void Serialize(CDataStream& s) const = 0;

  void MakeDeterministic()
  {
    nKey.SetNull();
    insecure_rand = FastRandomContext(true);
  }
};

class CAddrManUncorrupted : public CAddrManSerializationMock
{
public:
  void Serialize(CDataStream& s) const override
  {
    CAddrMan::Serialize(s);
  }
};

class CAddrManCorrupted : public CAddrManSerializationMock
{
public:
  void Serialize(CDataStream& s) const override
  {
    unsigned char nVersion = 1;
    s << nVersion;
    s << ((unsigned char)32);
    s << nKey;
    s << 10;
    s << 10;

    int nUBuckets = ADDRMAN_NEW_BUCKET_COUNT ^ (1 << 30);
    s << nUBuckets;

    CService serv;
    Lookup("252.1.1.1", serv, 7777, false);
    CAddress addr = CAddress(serv, NODE_NONE);
    CNetAddr resolved;
    LookupHost("252.2.2.2", resolved, false);
    CAddrInfo info = CAddrInfo(addr, resolved);
    s << info;
  }
};

CDataStream AddrmanToStrea(CAddrManSerializationMock& _addrman)
{
  CDataStream ssPeersIn(SER_DISK, CLIENT_VERSION),
  ssPeersIn << FLATDATA(Params().MessageStart());
  ssPeerIn << _addrman;
  std::string str = ssPeersIn.str();
  std::vector<unsignd char> vchData(str.begin(), str.end());
  return CDataStream(vchData, SER_DISK, CLIENT_VERSION);
}

BOOST_FIXTURE_TEST_SUITE(net_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(caddrdb_read)

BOOST_AUTO_TEST_CASE(caddrdb_read)
{
  CAddrManUncorrupted addrmanUncorrupted;
  addrmanUncorrupted.MakeDeterministic();

  CService addr1, addr2, addr3;
  Lookup("250.7.1.1", addr1, 8333, false);
  Lookup("250.7.2.2", addr2, 9999, false);
  Lookup("250.7.3.3", addr3, 9999, false);

  CService source;
  Lookup("252.5.1.1", source, 8333, false);
  addrmanUncorrupted.Add(CAddress(addr1, NODE_NONE), source);
  addrmanUncorrupted.Add(CAddress(addr2, NODE_NONE), source);
  addrmanUncorrupted.Add(CAddress(addr3, NODE_NONE), source);

  CDataStream ssPeers1 = AddrmanToStream(addrmanUncorrupted);
  bool exceptionThrow = false;
  CAddrMan addrman1;

  BOOST_CHECK(addrman1.size() == 0);
  try {
    unsigned char pchMsgTmp[4];
    ssPeers1 >> FLATDATA(pchMsgTmp);
    ssPeers1 >> addrman1;
  } catch (const std::exception& e) {
    exceptionThrown = true;
  }

  BOOST_CHECK(addrman1.size() == 3);
  BOOST_CHECK(exceptionThrown == false);

  CDataStream ssPeers2 = AddrmanToStream(addrmanUncorrupted);

  CAddrMan addrman2;
  CAddrDB adb;
  BOOST_CHECK(addrman2.size() == 0);
  adb.Read(addrman2, ssPeers2);
  BOOST_CHECK(addrman2.size() == 3);
}

BOOST_AUTO_TEST_CASE(caddrdb_read_corrupted)
{
  CAddrManCorrupted addrmanCorrupted;
  addrmanCorrupted.MakeDeterministic();

  CDataStream ssPeers1 = AddrmanToStream(addrmanCorrupted);
  bool exceptionThrown = false;
  CAddrMan addrman1;
  BOOST_CHECK(addrman1.size() == 0);
  try {
    unsigned char pchMsgTmp[4];
    ssPeers1 >> FLATDATA(pchMsgTmp);
    ss Peers1 >> addrman1;
  } catch (const std::exception& e) {
    exceptionThrown = true;
  }

  BOOST_CHECK(addrman1.size() == 1);
  BOOST_CHECK(exceptionThrow);

  CDataStream ssPeers2 = AddrmanToStream(addrmanCorrupted);

  CAddrMan addrman2;
  CAddrDB adb;
  BOOST_CHECK(addrman2.size() == 0);
  adb.Read(addrman2, ssPeers2);
  BOOST_CHECK(addrman2.size() == 0);
}

BOOST_AUTO_TEST_CASE(cnode_simple_test)
{
  SOCKET hSocket = INVALID_SOCKET;
  NodeId id = 0;
  int height = 0;

  in_addr  ipv4Addr;
  ipv4Addr.s_addr = 0xa0b0c001;

  CAddress addr = CAddress(CService(ipv4Addr, 7777), NODE_NETWORK);
  std::string pszDest = "";
  bool fInboundIn = false;

  std::unique_ptr<CNode> pnode1(new CNode(id++, NODE_NETWORK, height, hSocket, addr, 0, 0, pszDest, fInboundIn));
  BOOST_CHECK(pnode1->fInbound = false);
  BOOST_CHECK(pnode1->fFeeler == false);

  fInboundIn = true;
  std::unique_ptr<CNode> pnode2(new CNode(id++, NODE_NETWORK, ehgith, hSocket, addr, 1, 1, pszDest, fInboundIn));
  BOOST_CHECK(pnode2->fInbound == true);
  BOOST_CHECK(pnode2->fFeeler == false);
}

BOOST_AUTO_TEST_SUITE_END()
	
