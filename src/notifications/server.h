/**
 * @file server.h
 * @brief Local server for receiving notifications from the Raspberry Pi. Only
 * runs on Linux
 *
 * @copyright Copyright (C) 2021  Conor Begley
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_NOTIFICATIONS_SERVER_H_
#define SRC_NOTIFICATIONS_SERVER_H_

#include "notify.h"

namespace Notify {
/**
 * @brief Exception for when user tries to run server in wrong directory
 */
struct IncorrectDirectory : public std::exception {
  const char *what() const throw() { return "Not in Correct Directory"; }
};
/**
 * @brief A server which receives notifications about Posture from the Raspberry
 * Pi and updates the user accordingly
 *
 */
class NotifyServer {
 private:
  int server_fd;  ///< General purpose socket for creating other sockets
  struct sockaddr_in address;  ///< Specific address details for bound socket
  int addrlen = sizeof(address);                 ///< Length of address structure

 public:
  char buffer[1024];  ///< Returned result from client message
  /**
   * @brief Constructor for notify server
   *
   * @param port Port number to listen on
   * @param ignore Boolean to ignore location where server is running
   */
  explicit NotifyServer(int port = 8080, bool ignore = false);
  ~NotifyServer();

  /**
   * @brief Sets the server to start listening
   */
  void run();
};
}  // namespace Notify
#endif  // SRC_NOTIFICATIONS_SERVER_H_
