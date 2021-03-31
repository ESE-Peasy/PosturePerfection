/**
 * @file client_main.cpp
 * @brief Program for running a notification client, only runs on Linux
 *
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

int main(int argc, char *argv[]) {
  char *ip = const_cast<char *>("192.168.1.255");
  Notify::NotifyClient client(ip, 8080);
  std::string msg = "Hello World";
  client.sendMessage(msg);
  return 1;
}
