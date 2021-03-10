#include "server.h"

namespace Notify {

NotifyServer::NotifyServer(int port) {
  std::cout << "Starting server \n";
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  int opt = 1;
  int setup = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                         &opt, sizeof(opt));

  int addrlen = sizeof(address);
  int binding = bind(server_fd, (struct sockaddr *)&address, addrlen);
  int listening = listen(server_fd, 5);
}
NotifyServer::~NotifyServer() {
  shutdown(server_fd, SHUT_RDWR);
  close(server_fd);
}

void NotifyServer::run() {
  std::memset(buffer, 0, sizeof(buffer));

  int pi_fd =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

  int posture_value = read(pi_fd, buffer, 1024);
  std::string buff(buffer);
  std::string start("notify-send \"Posture Perfection\" \"");
  std::string end(
      "\" -u critical --icon "
      "/$HOME/code/Uni/Posture/PosturePerfection/docs/images/"
      "posture-logo-no-text.png");
  std::string out = start + buff + end;
  system(out.c_str());
  std::cout << buff;
}
// shutdown(pi_fd, SHUT_RDWR);
// close(pi_fd);
};  // namespace Notify
