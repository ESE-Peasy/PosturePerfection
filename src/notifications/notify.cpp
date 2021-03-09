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
}  // namespace Notify