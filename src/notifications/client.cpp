#include "client.h"

namespace Notify {
NotifyClient::NotifyClient(char* ip, int port) {
  this->server_ip = ip;
  this->client_fd = socket(AF_INET, SOCK_STREAM, 0);
  this->address.sin_family = AF_INET;
  this->address.sin_port = htons(port);
  int addrlen = sizeof(address);
}
NotifyClient::~NotifyClient() {
  int shut = shutdown(this->client_fd, SHUT_RDWR);
  Notify::err_msg(shut, "socket_shutdown");
  int cls = close(this->client_fd);
  Notify::err_msg(cls, "socket_close");
}

void NotifyClient::sendMessage(std::string msg) {
  int server_connect =
      inet_pton(AF_INET, this->server_ip, &(this->address.sin_addr));
  Notify::err_msg(server_connect, "server_address");
  int connection = connect(this->client_fd, (struct sockaddr*)&(this->address),
                           sizeof(this->address));

  send(this->client_fd, msg.c_str(), msg.length(), 0);
}
}  // namespace Notify
