#include <stdio.h>

#include <boost/test/unit_test.hpp>
#include <future>
#include <iostream>

#include "../src/notifications/broadcaster.h"
#include "../src/notifications/receiver.h"

// Currently cannot test sockets with Boost, so tests can only check if server
// handles checking in the right directory

BOOST_AUTO_TEST_CASE(CheckDirectory) {
  BOOST_CHECK_NO_THROW(Notify::NotifyReceiver serv(8080, true));
  BOOST_CHECK_THROW(Notify::NotifyReceiver serv(8080),
                    Notify::IncorrectDirectory);
}
