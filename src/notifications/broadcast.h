/**
 * @file client.h
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

#ifndef SRC_NOTIFICATIONS_BROADCAST_H_
#define SRC_NOTIFICATIONS_BROADCAST_H_
#include <string>

#include "notify.h"

namespace Notify {
/**
 * @brief A client which sends notifications about Posture to a server
 *
 */
class NotifyBroadcast {
 private:
  int broadcast_fd;            ///< Socket which the client is bound to
  struct sockaddr_in address;  ///< Client address structure
  char* target_ip;             ///< IP address of server to connect client too

 public:
  /**
   * @brief Constructor for broadcast client
   *
   * @param port Port number to listen on (default is 121121)
   * @param ip Address of server to connect to
   */
  explicit NotifyBroadcast(char* ip = (char*)"255.255.255.255",
                           int port = 121121);
  ~NotifyBroadcast();

  /**
   * @brief Sends message from client to server
   * @param msg The message to be sent
   */
  void sendMessage(std::string msg);
};
}  // namespace Notify
#endif  // SRC_NOTIFICATIONS_BROADCAST_H_
