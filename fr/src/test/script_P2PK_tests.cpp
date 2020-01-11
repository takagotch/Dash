
#include "script/script.h"
#include "test/test_dash.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(script_P2PK_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(IsPayToPublicKey)
{
  
  static const unsigned char p2pkcompressedeven[] = {
    0x41, 0x02, 0, 0, 0,0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OP_CHECKSIG
  };

}
