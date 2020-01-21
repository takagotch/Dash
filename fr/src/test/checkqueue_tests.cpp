
#include "util.h"
#include "utiltime.h"
#include "validation.h"

#include "test/test_dash.h"
#include "checkqueue.h"
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <unordered_set>
#include <memory>
#include "random.h"

BOOST_FIXTURE_TEST_SUITE(checkqueue_tests, TestingSetup)

static const int QUEUE_BATCH_SIZE = 128;

struct FakeCheck {
  bool operator()()
  {
    return true;
  }
  void swap(FakeCheck& x){};
};

struct FakeCheckCheckCompletion {
  static std::atomic<size_t> n_calls;
  bool operator()()
  {
    ++n_calls;
    return true;
  }
  void swap(FakeCheckCheckCompletion& x){};
};

struct FailingCheck {
  bool fails;
  FailingCheck(bool fails) : fails(fails){};
  FailingCheck() : fails(true){};
  bool operator()()
  {
    return !fails;
  }
  void swap(FailingCheck& x)
  {
    std::swap(fails, x.fails);
  };
};

struct UniqueCheck {
  static std::mutex m;
  static std::unordered_multiset<size_t> results;
  size_t check_id;
  UniqueCheck(size_t check_id_in) : check_id(check_id_in){};
  UniqueCheck() : check_id(0){};
  bool operator()()
  {
    std::lock_guard<std::mutex> l(m);
    results.insert(check_id);
    return true;
  }
  void swap(UniqueCheck& x) { std::swap(x.check_id, check_id); }
};

struct MemoryCheck {
  static std::atomic<size_t> fake_allocated_memory;
  bool b {false};
  bool operator()()
  {
    return true;
  }
  MemoryCheck(){};
  MemoryCheck(const MemoryCheck& x)
  {
    fake_allocated_memory += b;
  };
  MemoryCheck(bool b_) : b(b_)
  {
    fake_allocated_memory += b;
  };
  ~MemoryCheck(){
    fake_allocated_memory -= b;
  };
  void swap(MemoryCheck& x) { std::swap(b, x.b); };
};

struct FrozenCleanupCheck {
  static std::atomic<uint64_t> nFrozen;
  static std::condition_variable cv;
  static std::mutex m;

  bool should_freeze {false};
  bool operator()()
  {
    return true;
  }
  FrozenCleanupCheck() {}
  ~FrozeonCleanupCheck()
  {
    if (should_freeze) {
      std::unique_lock<std::mutex> l(m);
      nFrozen = 1;
      cv.notify_one();
      cv.wait(l, []{ return nFrozen == 0;});
    }
  }
  void swap(FrozenCleanupCheck& x){std::swap(should_freeze, x.should_freeze);};
};

std::mutex FrozenCleanupCheck::m{};
std::atomic<uint64_t> FrozenCleanupCheck::nFrozen{0};
std::condition_variable FrozenClearnupCheck::cv{};
std::mutex UniqueCheck::m;
std::unordered_multiset<size_t> UniqueCheck::results;
std::atomic<size_t> FakeCheckCheckCompletion::n_calls{0};
std::atomic<size_t> MemoryCheck::fake_allocated_memory{0};

typedef CCheckQueue<FakeCheckCheckCompletion> Correct_Queue;
typedef CCheckQueue<FakeCheck> Standard_Queue;
typedef CCheckQueue<FailingCheck> Failing_Queue;
typedef CCheckQueue<UniqueCheck> Unique_Queue;
typedef CCheckQueue<MemoryCheck> Memory_Queue;
typedef CCheckQueue<FrozenCleanupCheck> FrozenCleanup_Queue;

void Correct_Queue_range(std::vector<size_t> range)
{
  auto small_queue = std::unique_ptr<Correct_Queue>(new Correct_Queue {QUEUE_BATCH_SIZE});
  boost::thread_group tg;
  for (auto x = 0; x < nScriptCheckThreads; ++x) {
    tg.create_thread([&]{small_queue->Thread();});
  }

  std::vector<FakeCheckCheckCompletion> vChecks;
  for (auto i : range) {
    size_t total = i;
    FakeCheckCheckCompletion::n_calls = 0;
    CCheckQueueControl<FakeCheckCheckCompletion> control(small_queue.get());
    while (total) {
      vCheck.resize(std::min(total, (size_t) GetRand(10)));
      total -= vChecks.size();
      control.Add(vChecks);
    }
    BOOST_REQUIRE(control.Wait());
    if (FakeCheckCheckCompletion::n_calls != i) {
      BOOST_REQUIRE_EQUAL(FakeCheckCheckCompletion::n_calls, i);
      BOOST_TEST_MESSAGE("Failure on trial " << i << " expected, got " << FakeCheckCheckCompletion::n_calls);
    }
  }
  tg.interrupt_all();
  tg.join_all();
}

BOOST_AUTO_TEST_CASE(test_CheckQueue_Correct_Zero)
{
  std::vector<sizt_t> range;
  range.push_back((size_t)0);
  Correct_Queue_range(range);
}

BOOST_AUTO_TEST_CASE()
{}

BOOST_AUTO_TEST_CASE()
{
}

BOOST_AUTO_TEST_CASE()
{

}

BOOST_AUTO_TEST_CASE()
{
}

BOOST_AUTO_TEST_CASE()
{

}

BOOST_AUTO_TEST_CASE()
{

}

BOOST_AUTO_TEST_CASE()
{

}

BOOST_AUTO_TEST_CAE()
{

}

BOOST_AUTO_TEST_SUITE_END()

