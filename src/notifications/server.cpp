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
  this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
  this->address.sin_family = AF_INET;
  this->address.sin_addr.s_addr = INADDR_ANY;
  this->address.sin_port = htons(port);
  int opt = 1;
  int setup = setsockopt(this->server_fd, SOL_SOCKET,
                         SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

  int addrlen = sizeof(address);
  int binding =
      bind(this->server_fd, (struct sockaddr *)&(this->address), addrlen);
  Notify::err_msg(binding, "socket_bind");
  int listening = listen(this->server_fd, 5);
  Notify::err_msg(listening, "socket_listen");
}
NotifyServer::~NotifyServer() {
  shutdown(this->server_fd, SHUT_RDWR);
  close(this->server_fd);
}

void NotifyServer::run() {
  std::cout.flush();
  std::memset(this->buffer, 0, sizeof(this->buffer));

  int pi_fd = accept(this->server_fd, (struct sockaddr *)&(this->address),
                     reinterpret_cast<socklen_t *>(&(this->addrlen)));
  Notify::err_msg(pi_fd, "socket_accepting");
  int read_value = read(pi_fd, this->buffer, 1024);
  Notify::err_msg(read_value, "socket_reading");
  std::string buff(this->buffer);
  char current_d[1000];
  getcwd(current_d, sizeof(current_d));
  std::string cwd(current_d);
  std::string start("notify-send \"Posture Perfection\" \"");
  std::string middle("\" -u critical --icon ");
  std::string end(
      "/docs/images/"
      "posture-logo-no-text.png");
  std::string out = start + buff + middle + cwd + end;
  system(out.c_str());
  std::cout << out;
  shutdown(pi_fd, SHUT_RDWR);
  close(pi_fd);
}
};  // namespace Notify
