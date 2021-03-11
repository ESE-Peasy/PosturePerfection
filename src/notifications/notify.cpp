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
  const int max_buffer = 256;
  char buffer[max_buffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");

  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
    pclose(stream);
  }
  return data;
}

}  // namespace Notify