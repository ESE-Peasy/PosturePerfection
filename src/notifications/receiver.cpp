/**
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
#include "receiver.h"

#include <exception>

namespace Notify {

NotifyReceiver::NotifyReceiver(int port, bool ignore, bool check_env) {
  if (!ignore) {
    if (Notify::GetStringFromCommand("pwd | grep -o '[^/]*$'") !=
        "PosturePerfection\n") {
      std::cout
          << "Please run this in the root PosturePerfection Directory and not "
          << GetStringFromCommand("pwd");
      throw IncorrectDirectory();
    }
  }
  this->receive_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (this->receive_fd == 0) {
    Notify::err_msg(-1, "socket_creation");
  }
  this->address.sin_family = AF_INET;
  this->address.sin_addr.s_addr = htons(INADDR_ANY);
  this->address.sin_port = htons(port);
  int opt = 1;
  int setup =
      setsockopt(this->receive_fd, SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT | SO_BROADCAST, &opt, sizeof(opt));
  if (setup) {
    Notify::err_msg(-1, "socket_creation");
  }
  int binding = bind(this->receive_fd, (struct sockaddr *)&(this->address),
                     sizeof(this->address));
  Notify::err_msg(binding, "socket_bind");
  if (check_env == true) {
    char *env = getenv("XDG_CURRENT_DESKTOP");
    if (std::strcmp(env, "ubuntu:GNOME") == 0) {
      this->command = "notify-send -u critical \"Posture Perfection\" \"";
    }
  }
}
NotifyReceiver::~NotifyReceiver() {
  shutdown(this->receive_fd, SHUT_RDWR);
  close(this->receive_fd);
}

void NotifyReceiver::run() {
  std::cout.flush();
  std::memset(this->buffer, 0, sizeof(this->buffer));
  std::memset(&this->broadcaster_address, 0, sizeof(broadcaster_address));

  int read_value =
      recvfrom(this->receive_fd, this->buffer, 1024, MSG_DONTWAIT,
               (struct sockaddr *)&this->broadcaster_address,
               reinterpret_cast<socklen_t *>(&this->broadcaster_len));
  if (read_value > 0) {
    char current_d[1024];
    getcwd(current_d, sizeof(current_d));
    std::string cwd(current_d);
    std::string middle("\" --icon ");
    std::string end(
        "/docs/images/"
        "posture-logo-no-text.png");
    this->buffer[read_value] = '\0';
    std::string out = this->command + this->buffer + middle + cwd + end;
    system(out.c_str());
  }
}
};  // namespace Notify
