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

#ifndef SRC_NOTIFICATIONS_CLIENT_H_
#define SRC_NOTIFICATIONS_CLIENT_H_

#include <stdio.h>

#include <string>

namespace Notify {
/**
 * @brief A client which sends notifications about Posture to a server
 *
 */
class NotifyClient {
 private:
  /**
   * @brief socket client is bound to
   */
  int client_fd;

 public:
  /**
   * @brief Constructor for notify client
   *
   * @param port port number to listen on (default is 8080)
   * @param string hostname (default is raspberrypi.local)
   * @param ip address of raspberry pi (if set to any value,
   * hostname string is ignored)
   */
  NotifyClient(std::string hostname = "raspberrypi.local", int port = 8080,
               char* ip = nullptr);
  ~NotifyClient();

  /**
   * @brief Sends message from client to server
   * @param message the message to be sent
   */
  void sendMessage(std::string message);
};
}  // namespace Notify
#endif  // SRC_NOTIFICATIONS_CLIENT_H_
