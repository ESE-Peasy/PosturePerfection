/**
 * @file notify.h
 * @brief Shared functions between `server.h` and `client.h`. Only runs on Linux
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

#ifndef SRC_NOTIFICATIONS_NOTIFY_H_
#define SRC_NOTIFICATIONS_NOTIFY_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

/**
 * @brief Responsible for enabling the sending of notifications from the
 * Raspberry Pi to a secondary device.
 *
 */
namespace Notify {
/**
 * @brief Error handler for socket connections
 *
 * @param num The result of socket function
 * @param msg Message to be printed also default error message
 */
int err_msg(int num, std::string msg);

/**
 * @brief Return result from system command as a string
 * @param cmd The system command
 */
std::string GetStringFromCommand(std::string cmd);
};      // namespace Notify
#endif  // SRC_NOTIFICATIONS_NOTIFY_H_
