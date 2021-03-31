/**
 * @file server.cpp
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
#include "server.h"

#include <exception>
#include <string>

namespace Notify {

NotifyServer::NotifyServer(int port, bool ignore) {
  if (!ignore) {
    if (Notify::GetStringFromCommand("pwd | grep -o '[^/]*$'") !=
        "PosturePerfection\n") {
      std::cout
          << "Please run this in the root PosturePerfection Directory and not "
          << GetStringFromCommand("pwd");
      throw IncorrectDirectory();
    }
  }
  std::cout << "The IP for this device is: \n"
            << Notify::GetStringFromCommand("hostname -I | grep -Eo '^[^ ]+'");
  this->server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  this->address.sin_family = AF_INET;
  this->address.sin_addr.s_addr = INADDR_ANY;
  this->address.sin_port = htons(port);
  int opt = 1;
  int setup = setsockopt(this->server_fd, SOL_SOCKET,
                         SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

  int addr_len = sizeof(address);
  int binding = bind(this->server_fd, (struct sockaddr *)&(this->address),
                     sizeof(this->address));
  Notify::err_msg(binding, "socket_bind");
}
NotifyServer::~NotifyServer() {
  shutdown(this->server_fd, SHUT_RDWR);
  close(this->server_fd);
}

void NotifyServer::run() {
  std::cout.flush();
  std::memset(this->buffer, 0, sizeof(this->buffer));
  std::memset(&this->client_address, 0, sizeof(client_address));

  int read_value = recvfrom(this->server_fd, this->buffer, 1024, MSG_DONTWAIT,
                            (struct sockaddr *)&this->client_address,
                            (socklen_t *)(&this->client_len));
  ;
  if (read_value > 0) {
    char current_d[1024];
    getcwd(current_d, sizeof(current_d));
    std::string cwd(current_d);
    std::string start("notify-send \"Posture Perfection\" \"");
    std::string middle("\" -u critical --icon ");
    std::string end(
        "/docs/images/"
        "posture-logo-no-text.png");
    this->buffer[read_value] = '\0';
    std::string out = start + this->buffer + middle + cwd + end;
    system(out.c_str());
    std::cout << out;
  };
}
};  // namespace Notify
