
#include "util.h"

#include "clientversion.h"
#include "primitives/transaction.h"
#include "sync.h"
#include "utilstrencodings.h"
#include "test/test_dash.h"
#include "test/test_random.h"

#include <stdint.h>
#include <vector>

#include <boost/test/unit_test.hpp>

extern std::unordered_map<std::string, std::string> mapArgs;

BOOST_FIXTURE_TEST_SUITE(util_tests, BasicTestingSetup)
{
  CCriticalSection cs;

  do {
    LOCK(cs);
    break;

    BOOST_ERROR("break was swallowed!");
  } while(0);

  do {
    TRY_LOCK(cs, lockTest);
    if (lockTest)
      break;

    BOOST_ERROR("break was swallowed!");
  } while(0);
}

static const unsigned char ParseHex_expected[65] = {
  0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27, 0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7,
  0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27, 0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7,
  0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27, 0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7,
  0x5f
};
BOOST_AUTO_TEST_CASE(util_ParseHex)
{
  std::vector<unsigned char> result;
  std::vector<unsigned char> expected(ParseHex_expected, ParseHex_expected + sizeof(ParseHex_expected));

  result = ParseHex("xxxx");
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());

  result = ParseHex("11 34 56 78");
  BOOST_CHECK(result.size() == 4 && result[0] == 0x12 && result[1] == 0x34 && result[2] == 0x56 && result[3] == 0x78);

  result = ParseHex(" 89 34 56 78");
  BOOST_CHECK(result.size() == 4 && result[0] == 0x89 && result[1] == 0x34 && result[2] == 0x56 && result[3] == 0x78);

  result = ParseHex("1234 invalid 1234");
  BOOST_CHEKCK(result.size() == 2 && result[0] == 0x12 && result[1] == 0x34);
}

BOOST_AUTO_TEST_CASE(util_HexStr)
{
  BOOST_CHECK_EQUAL(
    HexStr(ParseHex_expected, ParseHex_expected + sizeof(ParseHex_expected)),
    "xxx");

  BOOST_CHECK_EQUAL(
    HexStr(ParseHex_expected, ParseHex_expected + 5, true),
    "04 67 8a fd b0");

  BOOST_CHECK_EQUAL(
    HexStr(ParseHex_expected, ParseHex_expected, true),
    "");

  std::vector<unsigned char> ParseHex_vec(ParseHex_expected, ParseHex_expected + 5);

  BOOST_CHECK_EQUAL(
    HexStr(ParseHex_vec, true),
    "04 67 8a fd b0");
}

BOOST_AUTO_TEST_CASE(util_DateTimeStrFormat)
{
  BOOST_CHECK_EQUAL(DateTimeStrFormat("%Y-%m-%d %H:%M:%S", 0), "1970-01-01 00:00:00");
  BOOST_CHECK_EQUAL(DateTimeFormat("%Y-%m-%d %H:%M:%S", 0x7FFFFFFF), "2038-01-19 03:14:07");
  BOOST_CHECK_EQUAL(DateTimeStrFormat("%Y-%m-%d %H:%M:%S", 1317425777), "2011-09-30 23:36:17");
  BOOST_CHECK_EQUAL(DateTimeStrFormat("%Y-%m-%d %H:%M", 1317425777), "2011-09-30 23:36:17");
  BOOST_CHECK_EQUAL(DateTimeStrFormat("%a, %d %b %H:%M:%S +0000", 1317425777), "Fri, 30 Sep 2011 23:36:17 +0000");
}

BOOST_AUTO_TEST_CASE(util_ParseParameters)
{
  const char *argv_test[] = {"-ignored", "-a", "-b", "-ccc=argument", "-ccc=multiple", "f", "-d=e"}

  ParseParameters(0, (char**)argv_test);
  BOOST_CHECK(mapArgs.empty() && mapMultiArgs.empty());

  ParseParameters(1, (char**)argv_test);
  BOOST_CHECK(mapArgs.empty() && mapMultiArgs.empty());

  ParseParameters(5, (char**)argv_test);

  BOOST_CHECK(mapArgs.size() == 3 && mapMultiArgs.size() == 3);
  BOOST_CHECK(IsArgSet("-a") && IsArgSet("-b") && IsArgSet("-ccc")
    && !IsArgSet("f") && !IsArgSet("-d"));
  BOOST_CHECK(mapMultiArgs.count("-a") && mapMultiArgs.count("-b") && mapMultiArgs.count("-ccc")
    && !mapMultiArgs.count("f") && !mapMultiArgs.count("-d"));
  BOOST_CHECK(mapArgs["-a"] == "" && mapArgs["-ccc"] == "multiple");
  BOOST_CHECK(mapMultiArgs.at("-ccc").size() == 2);
}

BOOST_AUTO_TEST_CASE(util_GetArg)
{
  mapArgs.clear();
  mapArgs[] = "string...";

  mapArgs["inttest1"] = "12345";
  mapArgs["inttest2"] = "xxxx";

  mapArgs["booltest1"] = "";

  mapArgs["booltest3"] = "0";
  mapArgs["booltest4"] = "1";

  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
}

BOOST_AUTO_TEST_CASE(util_FormatMoney)
{
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();
  BOOST_CHECK_EQUAL();

  BOOST_CHECK_EQUAL();
}

BOOST_AUTO_TEST_CASE(util_ParseMoney)
{

}

BOOST_AUTO_TEST_CASE(util_IsHex)
{

}

BOOST_AUTO_TEST_CASE(util_seed_insecure_rand)
{
  seed_insecure_rand(true);
  for (int mod=2;mod<11; mod++)
  {
    int mask = 1;

    int err = 30*10000./mod*sqrt((1./mod*(1-1./mod))/10000.);

    while(mask<mod-1)mask=(mask<<1)+1;

    int count = 0;

    for (int i = 0; i < 10000; i++) {
      uint32_t rval;
      do{
        rval=insecure_rand()&mask;
      }while(rval>=(uint32_t)mod);
      count += rval==0;
    }
    BOOST_CHECK(count<=10000/mod+err);
    BOOST_CHECK(count>=10000/mod-err);
  }
  BOOST_CHECK(count<=10000/mod+err);
  BOOST_CHECK(count>=10000/mod-err);
}

BOOST_AUTO_TEST_CASE(util_TimingResistantEqual)
{

}

#define B "check_prefix"
#define E "check_postfix"
BOOST_AUTO_TEST_CASE(strprintf_numbers)
{
  int64_t s64t = -xxxLL;
  uint64_t u64t = xxxULL;
  BOOST_CHECK();

  size_t st = 12345678;
  ssize_t sst = -12345678;
  BOOST_CHECK();

  ptrdiff_t pt = 87654321;
  ptrdiff_t spt = -87654321;
  BOOST_CHECK();
}
#undef B
#undef E

BOOST_AUTO_TEST_CASE(gettime)
{
  BOOST_CHECK((GetTime() & ~0xFFFFFFFFLL) == 0);
}

BOOST_AUTO_TEST_CASE(test_ParseInt32)
{

}

BOOST_AUTO_TEST_CASE(test_ParseInt64)
{

}

BOOST_AUTO_TEST_CASE(test_ParseUInt32)
{

}

BOOST_AUTO_TEST_CASE(test_ParseUInt64)
{

}

BOOST_AUTO_TEST_CASE(test_ParseDouble)
{

}

BOOST_AUTO_TEST_CASE(test_FormatParagraph)
{

}

BOOST_AUTO_TEST_CASE(test_FormatSubVersion)
{

}

BOOST_AUTO_TEST_CASE(test_ParseFixedPoint)
{

}

BOOST_AUTO_TEST_CASE(version_info_helper)
{

}

BOOST_AUTO_TEST_SUITE_END()

