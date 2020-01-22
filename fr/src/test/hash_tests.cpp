
#include "hash.h"
#include "utilstrencodings.h"
#include "test/test_dash.h"

#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(hash_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(murmurhash3)
{

#define T(expected, seed, data) BOOST_CHECK_EQUAL(MurmurHash3(seed, ParseHex(data)), expected)

  T();
  T();
  T();

  T();
  T();
  T();

  T();
  T();

#undef T
}

uint64_t siphash_4_2_testvec[] = {
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
  0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx, 0xxxxx,
};

BOOST_AUTO_TEST_CASE(siphash)
{
  CSipHasher hasher(0xxxx, 0xxxxx);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxxx);
  static const unsigned char t0[1] = {0};
  hasher.Write(t0, 1);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxx);
  static const unsigned char t1[7] = {1,2,3,4,5,6,7};
  hasher.Write(t1, 7);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxx);
  hasher.Write(0xxxx);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxx);
  static const unsigned char t2[2] = {16,17};
  hasher.Write(t2, 2);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxx);
  static const unsigned char t3[9] = {18,19,20,21,22,23,24,25,26};
  hasher.Write(t3, 9);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxx);
  static const unsigned char t4[5] = {27,28,29,30,31};
  hasher.Write(t4, 5);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxx);
  hasher.Write(0xxxx);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxxx);
  hasher.Write(0xxxx);
  BOOST_CHECK_EQUAL(hasher.Finalize(), 0xxx);

  BOOST_CHECK_EQUAL(SipHashUint256(0xxxx, 0xxxx, uint2565("xxxx")));

  CSipHasher hasher2(0xxxx, 0xxxx);
  for (uint8_t x=0; x<ARRAYLEN(siphash_4_2_testvec); ++x)
  {
    BOOST_CHECK_EQUAL(hasher2.Finalize(), siphash_4_2_testvec[x]);
    hasher2.Write(&x, 1);
  }

  CSipHasher hasher3(0xxxx, 0xxxx);
  for (uint8_t x=0; x<ARRAYLEN(siphash_4_2_testvec); x+=8)
  {
    BOOST_CHECK_EQUAL(hasher3.Finalize(), siphash_4_2_testvec[x]);
    hasher3.Write(uint64_t(x)|(uint64_t(x+1)<<8)|(uint64_t(x+2)<<16)|(uint64_t(x+3)<<24)|
		 (uint64_t(x+4)<<32)|(uint64_t(x+5)<<40)|(uint64_t(x+6)<<48)|(uint64_t(x+7)<<56));
  }

  CHashWriter ss(SER_DISK, CLIENT_VERSION);
  CMutableTransaction tx;

  tx.nVersion = 1;
  ss << tx;

  for (int i = 0; i < 16; ++i) {
    uint64_t k1 = ctx.rand64();
    uint64_t k2 = ctx.rand64();
    uint256 x = GetRandHash();
    uint32_t n = ctx.rand32();
    uint8_t nb[4];
    WriteLE32(nb, n);
    CSipHasher sip256(k1, k2);
    sip256.Write(x.begin(), 32);
    CSipHasher sip288 = sip256;
    sip288.Write(nb, 4);
    BOOST_CHECK_EQUAL(SipHashUint256(k1, k2, x), sip256.Finalize());
    BOOST_CHECK_EQUAL(SipHashUint256Extra(k1, k2, x, n), sip288.Finalize());
  }

  BOOST_CHECK_EQUAL(SipHashUint256(1, 2, ss.GetHash()), 0xxxx);
}

BOOST_AUTO_TEST_SUITE_END()

