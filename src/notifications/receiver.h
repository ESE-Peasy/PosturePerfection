/**
 * @file receiver.h
 * @brief Receiver for listening and receiving notifications from
 * `Notify::NotifyBroadcast`
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

#include <string>

#include "notify.h"

namespace Notify {
/**
 * @brief Exception for when user tries to run `Notify::NotifyReceiver` in wrong
 * directory
 */
struct IncorrectDirectory : public std::exception {
  const char *what() const throw() { return "Not in Correct Directory"; }
};
/**
 * @brief A `Notify::NotifyReceiver` for listening and receiving notifications
 * from `Notify::NotifyBroadcast`
 *
 */
class NotifyReceiver {
 private:
  int receive_fd;              ///< Receiver socket file descriptor
  struct sockaddr_in address,  ///<  Receiver address
      broadcaster_address;  ///<  Specific address details for received messages
  int addr_len = sizeof(address);  ///<  Length of address structure
  int broadcaster_len = sizeof(
      broadcaster_address);  ///<  Length of broadcaster address structure
  std::string command =
      "notify-send -t 0 \"Posture Perfection\" \"";  ///< Notify-send command
                                                     ///< and flags

 public:
  char buffer[1024];  ///< Returned result from broadcasted message
  /**
   * @brief Constructor for `Notify::NotifyReceiver`
   *
   * @param port Port number to listen on (default is 121121)
   * @param ignore Boolean to ignore location where Notify::NotifyReceiver is
   * running
   * @param check_env If `true` will check current desktop environment and
   * change notify-send command if needed
   */
  explicit NotifyReceiver(int port = 121121, bool ignore = false,
                          bool check_env = true);
  ~NotifyReceiver();

  /**
   * @brief Sets the `Notify::NotifyReceiver` to start listening
   */
  void run();
};
}  // namespace Notify
#endif  // SRC_NOTIFICATIONS_RECEIVER_H_
