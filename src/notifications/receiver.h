/**
 * @file receiver.h
 * @brief Local receiver for receiving notifications from the Raspberry Pi. Only
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

#ifndef SRC_NOTIFICATIONS_RECEIVER_H_
#define SRC_NOTIFICATIONS_RECEIVER_H_

#include "notify.h"

namespace Notify {
/**
 * @brief Exception for when user tries to run receiver in wrong directory
 */
struct IncorrectDirectory : public std::exception {
  const char *what() const throw() { return "Not in Correct Directory"; }
};
/**
 * @brief A receiver which listens for notifications about Posture from the
 * Raspberry Pi and updates the user accordingly
 *
 */
class NotifyReceiver {
 private:
  int receive_fd;              // Receiver socket file descriptor
  struct sockaddr_in address,  // Receiver address
      client_address;          // Specific address details for received messages
  int addr_len = sizeof(address);  // Length of address structure
  int client_len =
      sizeof(client_address);  // Length of broadcast address structure

 public:
  char buffer[1024];  // Returned result from broadcast message
  /**
   * @brief Constructor for notify receiver
   *
   * @param port Port number to listen on (default is 121121)
   * @param ignore Boolean to ignore location where receiver is running
   */
  explicit NotifyReceiver(int port = 121121, bool ignore = false);
  ~NotifyReceiver();

  /**
   * @brief Sets the receiver to start listening
   */
  void run();
};
}  // namespace Notify
#endif  // SRC_NOTIFICATIONS_RECEIVER_H_
