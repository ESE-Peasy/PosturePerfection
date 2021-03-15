#include <stdio.h>

#include <boost/test/unit_test.hpp>
#include <future>
#include <iostream>

#include "../src/notifications/client.h"
#include "../src/notifications/server.h"

// Currently cannot test sockets with Boost, so tests can only check if server
// handles checking in the right directory

BOOST_AUTO_TEST_CASE(CheckDirectory) {
  BOOST_CHECK_NO_THROW(Notify::NotifyServer serv(8080, true));
  BOOST_CHECK_THROW(Notify::NotifyServer serv(8080),
                    Notify::IncorrectDirectory);
}
