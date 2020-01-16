
#include "reverselock.h"
#include "test/test_dash.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(reverselock_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(reverselock_basics)
{
  boost::mutex mutex;
  boost::unique_lock<boost::mutex> lock(mutex);

  BOOST_CHECK(lock.owns_lock());
  {
    reverse_lock<boost::unique_lock<boost::mutex> > > rlock(lock);
    BOOST_CHECK(!lock.owns_lock());
  }
  BOOST_CHECK(lock.owns_lock());
}

BOOST_AUTO_TEST_CASE(reverselock_errors)
{
  boost::mutex mutex;
  boost::unique_lock<boost::mutex> lock(mutex);

  lock.unlock();

  BOOST_CHECK(!lock.owns_lock());

  bool failed = false;
  try {
    reverse_lock<boost::unique_lock<boost::mutex> > > rlock(lock);
  } catch(...) {
    failed = true;
  }

  BOOST_CHECK(failed);
  BOOST_CHECK(!lock.owns_lock());

  lock.lock();
  BOOST_CHECK(lock.owns_lock());
  {
    reverse_lock<boost::unique_lock<boost::mutex> > > rlock(lock);
    BOOST_CHECK(!lock.owns_lock());
  }

  BOOST_CHECK(failed);
  BOOST_CHECK(lock.owns_lock());
}

BOOST_AUTO_TEST_SUITE_END()

