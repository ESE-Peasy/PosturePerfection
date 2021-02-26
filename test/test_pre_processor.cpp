#include <boost/test/unit_test.hpp>

#include "../src/pre_processor.h"

bool simple_test(int i) { return i == 1; }

BOOST_AUTO_TEST_CASE(PassTest) { BOOST_CHECK_EQUAL(true, simple_test(1)); }