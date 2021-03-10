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
 * @brief A server which receives notifications about Posture from the Raspberry
 * Pi and updates the user accordingly
 *
 */
class NotifyServer {
 private:
  /**
   * @brief General purpose socket for creating other sockets
   */
  int server_fd;

  /**
   * @brief Specific address details for bound socket
   */
  struct sockaddr_in address;
  int addrlen;
  char buffer[1024];

 public:
  /**
   * @brief Constructor for notify server
   *
   * @param port port number to listen on
   */
  NotifyServer(int port);
  ~NotifyServer();

  /**
   * @brief Sets the server to start listening
   */
  void run();
};
}  // namespace Notify
#endif  // SRC_NOTIFICATIONS_SERVER_H_
