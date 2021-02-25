#define BOOST_TEST_MODULE DoublerTests
#include <boost/test/unit_test.hpp>

#include "../src/doubler.h"

BOOST_AUTO_TEST_CASE(PassTest) {
  int in[] = {-1, 0, 2};
  int expected_out[] = {-2, 0, 4};

  for (int i = 0; i < sizeof(in) / sizeof(in[0]); i++) {
    BOOST_CHECK_EQUAL(expected_out[i], doubler(in[i]));
  }
}
