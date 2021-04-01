/**
 * @file Broadcast.cpp
 * @brief Broadcast for sending notifications from the Raspberry Pi.
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
#include "broadcast.h"

namespace Notify {
NotifyBroadcast::NotifyBroadcast(char *ip, int port) {
  this->target_ip = ip;
  this->broadcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
  this->address.sin_family = AF_INET;
  this->address.sin_port = htons(port);
  this->address.sin_addr.s_addr = inet_addr(this->target_ip);
  int addrlen = sizeof(address);
  int opt = 1;
  int setup = setsockopt(this->broadcast_fd, SOL_SOCKET, SO_BROADCAST, &opt,
                         sizeof(opt));
}
NotifyBroadcast::~NotifyBroadcast() {
  int cls = close(this->broadcast_fd);
  Notify::err_msg(cls, "socket_close");
}

void NotifyBroadcast::sendMessage(std::string msg) {
  sendto(this->broadcast_fd, msg.c_str(), msg.length(), MSG_DONTWAIT,
         (const struct sockaddr *)&this->address, sizeof(this->address));
}
}  // namespace Notify
