
#include "timedata.h"
#include "test/test_dash.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(timedata_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(util_MedianFilter)
{
  CMedianFilter<int> filter(5, 15);

  BOOST_CHECK_EQUAL(filter.median(), 15);

  filter.input(20);
  BOOST_CHECK_EQUAL(filter.median(), 17);

  filter.input(30);
  BOOST_CHECK_EQUAL(filter.median(), 20);

  filter.input(3);
  BOOST_CHECK_EQUAL(filter.median(), 17);

  filter.input(7);
  BOOST_CHECK_EQUAL(filter.median(), 15);

  filter.input(7);
  BOOST_CHECK_EQUAL(filter.median(), 18);

  filter.input(0);
  BOOST_CHECK_EQUAL(filter.median(), 7);
}

