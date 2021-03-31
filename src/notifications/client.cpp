/**
 * @file client.cpp
 * @brief Local server for receiving notifications from the Raspberry Pi.
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
#include "client.h"

namespace Notify {
NotifyClient::NotifyClient(char *ip, int port) {
  this->server_ip = ip;
  this->client_fd = socket(AF_INET, SOCK_DGRAM, 0);
  this->address.sin_family = AF_INET;
  this->address.sin_port = htons(port);
  int addrlen = sizeof(address);
}
NotifyClient::~NotifyClient() {
  int cls = close(this->client_fd);
  Notify::err_msg(cls, "socket_close");
}

void NotifyClient::sendMessage(std::string msg) {
  int server_connect =
      inet_pton(AF_INET, this->server_ip, &(this->address.sin_addr));
  Notify::err_msg(server_connect, "server_address");
  sendto(this->client_fd, msg.c_str(), msg.length(), MSG_DONTWAIT,
         (const struct sockaddr *)&this->address, sizeof(this->address));
}
}  // namespace Notify
