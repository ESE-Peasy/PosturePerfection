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
#include "notify.h"

namespace Notify {
int err_msg(int num, std::string msg) {
  if (num < 0) {
    std::cerr << "Error with " << msg << "\n"
              << "Error Code:" << errno << "\n";
    exit(1);
  }
  return 0;
}

std::string GetStringFromCommand(std::string cmd) {
  std::string data;
  FILE* stream;
  const int MaxBuffer = 256;
  char buffer[MaxBuffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");

  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, MaxBuffer, stream) != NULL) data.append(buffer);
    pclose(stream);
  }
  return data;
}

}  // namespace Notify
