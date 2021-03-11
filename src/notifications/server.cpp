#include "server.h"

namespace Notify {

NotifyServer::NotifyServer(int port) {
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
  Notify::err_msg(binding, "socket_listen");
}
NotifyServer::~NotifyServer() {
  shutdown(this->server_fd, SHUT_RDWR);
  close(this->server_fd);
}

void NotifyServer::run() {
  std::memset(buffer, 0, sizeof(buffer));

  int pi_fd = accept(this->server_fd, (struct sockaddr *)&(this->address),
                     (socklen_t *)&(this->addrlen));

  int read_value = read(pi_fd, buffer, 1024);
  Notify::err_msg(read_value, "socket_reading");
  std::string buff(buffer);
  char current_d[1000];
  getcwd(current_d, sizeof(current_d));
  std::string cwd(current_d);
  std::string start("notify-send \"Posture Perfection\" \"");
  std::string middle("\" -u critical --icon ");
  std::string end(
      "/docs/images/"
      "posture-logo-no-text.png");
  std::string out = start + buff + middle + cwd + end;
  std::cout << out;
  system(out.c_str());
}
// shutdown(pi_fd, SHUT_RDWR);
// close(pi_fd);
};  // namespace Notify
