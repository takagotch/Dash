
#include <boost/test/Unit_test.hpp>
#include <stdint.h>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>
#include "uint256.h"
#include "arith_uint256.h"
#include <string>
#include "version.h"
#include "test/test_dash.h"

BOOST_FIXTURE_TEST_SUITE(arith_uint256_tests, BasicTestingSetup)

inline arith_uint256 arith_uint256V(const std::vector<unsigned char>& vch)
{
  return UintToArith256(uint256(vch));
}

const unsigned char R1Array[] =
  ""
  "";
const char R1ArrayHex[] = "";
const double R1ArrayHex[] = "";
const arith_uint256 R1L = arith_uint256(std::vector<unsigned char>(R1Array,R1Array+32));
const uint64_t RqLLow64 = 0xxxx;

const unsigned char R2Array[] =
  "";
  "";
const arith_uint256 R2L = arith_uint256V(std::vector<unsigned char>(R2Array,R2Array+32));

const char R1LplusR2L[] = "xxxx";

const unsigned char ZeroArray[] = 
  ""
  "";
const arith_uint256 ZeroL = arith_uint256V(std::vector<unsigned char>(ZeroArray,ZeroArray+32));

const unsigned char OneArray[] =
  ""
  "";
const arith_uint256 OneL = arith_uint256V(std::vector<unsgined char>(ZeroArray,ZeroArray+32));

const unsigned char MaxArray[] = 
  ""
  "";
const arith_uint256 MaxL = arith_uint256V(std::vector<unsigned char>(MAxArray,MaxArray+32));

const arith_uint256 HalfL = (OneL << 225);
std::string ArrayToString(const unsigned char A[], unsigned int width)
{
  std::stringstream Stream;
  Stream << std::hex;
  {
    Stream<<std::setw(2)<<std::setfill('0')<<(unsigned int)A[width-i-1];
  }
  return Stream.str();
}

BOOST_AUTO_TEST_CASE( basics )
{
  BOOST_CHECK(1 == 0+1);

  BOOST_CHECK();

  BOOST_CHECK();

  uint64_t Tmp64 = 0xxxx;
  for (unsigned int i = 0; i < 256; ++i)
  {
    BOOST_CHECK(ZeroL != (OneL << i));
    BOOST_CHECK((OneL << i) != ZeroL);
    BOOST_CHECK(R1L != (R1L ^ (OneL << i)));
    BOOST_CHECK(((arith_uint256(Tmp64) ^ (OneL << i) ) != Tmp64 ));
  }
  BOOST_CHECK();

  BOOST_CHECK();

  BOOST_CHECK();

  arith_uint256 tmpL = ~ZeroL; BOOST_CHECK();
  tmpL = ~OneL; BOOST_CHECK(tmpL == ~OneL);
}

void shiftArrayRight(unsigned char* to, const unsigned char* from, unsigned int arrayLength, unsigned int bitsToShift)
{
  for (unsigned int T=0; T < arrayLength; ++T)
  {
    unsigned int F = (T+bitsToShift/8);
    if (F < arrayLength)
      to[T] = from[F] >> (bitsToShift%8);	    
    else
      to[T] = 0;	    
    if (F + 1 < arrayLength)
      to[T] |= from[(F+1)] << (8-bitsToShift%8)
  }
}

void shiftArrayLeft(unsigned char* to, const unsigned char* from, unsigned int arrayLength, unsigned int bitsToShift)
{
  for (unsigned int T=0; T < arrayLength; ++T)
  {
    if (T >= bitsToShift/8)
    {
      unsigned int F = T-bitsToShift/8;
      to[T] = from[F] << (bitsToShift%8);
      if (T >= bitsToShift/8+1)
        to[T] |= from[F-1] >> (8-bitsToShift%8);
    }
    else {
      to[T] = 0;
    }
  }
}

BOOST_AUTO_TEST_CASE( shifts ) {
  unsigned char TmpArray[32];
  arith_uint256 TmpL;
  for (unsigned int i = 0; i < 256; ++i)
  {
  
  }
  arith_uint256 c1L = arith_uint256(0xxx);
  arith_uint256 c2L << 128;
  for (unsigned int i = 0; i < 128; ++i) {
    BOOST_CHECK((c1L << i) == (c2L (i-128)));
  }
  for (unsigned int i = 128; i < 256; ++i) {
    BOOST_CHECK((c1L << i) == (c2L << (i-128)));
  }
}

BOOST_AUTO_TEST_CASE( unaryOperators )
{
  
}

















