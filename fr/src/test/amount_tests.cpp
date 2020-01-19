
#include "amount.h"
#include "test/test_dash.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(amount_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(GetFeeTest)
{
  CFeeRate feeRate;

  feeRate = CFeeRate(0);

  BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1e5), 0);

  BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1e5), 0);

  feeRate = CFeeRate(1000);

  BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1), -1);
  BOOST_CHECK_EQUAL(feeRate.GetFee(121), -121);
  BOOST_CHECK_EQUAL(feeRate.GetFee(999), -999);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1e3), -1e3);
  BOOST_CHECK_EQUAL(feeRate.GetFee(9e3), -9e3);

  feeRate = CFeeRate(-1000);

  BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1), -1);
  BOOST_CHECK_EQUAL(feeRate.GetFee(121), -121);
  BOOST_CHECK_EQUAL(feeRate.GetFee(999), -999);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1e3), -1e3);
  BOOST_CHECK_EQUAL(feeRate.GetFee(9e3), -9e3);

  feeRate = CFeeRate(123);

  BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
  BOOST_CHECK_EQUAL(feeRate.GetFee(8), 1);
  BOOST_CHECK_EQUAL(feeRate.GetFee(9), 1);
  BOOST_CHECK_EQUAL(feeRate.GetFee(121), 14);
  BOOST_CHECK_EQUAL(feeRate.GetFee(122), 15);
  BOOST_CHECK_EQUAL(feeRate.GetFee(999), 122);
  BOOST_CHECK_EQUAL(feeRate.GetFee(1e3), 123);
  BOOST_CHECK_EQUAL(feeRate.GetFee(9e3), 1107);

  feeRate = CFeeRate(-123);

  BOOST_CHECK_EQUAL(feeRate.GetFee(0), 0);
  BOOST_CHECK_EQUAL(feeRate.GetFee(8), -1);
  BOOST_CHECK_EQUAL(feeRate.GetFee(9), -1);

  BOOST_CHECK(CFeeRate(CAmount(-1), 1000) == CFeeRate(-1));
  BOOST_CHECK(CFeeRate(CAmount(0), 1000) == CFeeRate(0));
  BOOST_CHECK(CFeeRate(CAmount(1), 1000) == CFeeRate(1));

  BOOST_CHECK(CFeeRate(CAmount(1), 1001) == CFeeRate(0));
  BOOST_CHECK(CFeeRate(CAmount(2), 1001) == CFeeRate(1));

  BOOST_CHECK(CFeeRate(CAmount(26), 789) == CFeeRate(32));
  BOOST_CHECK(CFeeRate(CAmount(27), 789) == CFeeRate(34));

  CFeeRate(MAX_MONEY, std::numeric_limits<size_t>::max() >> 1).GetFeePerK();
}

BOOST_AUTO_TEST_SUITE_END()

