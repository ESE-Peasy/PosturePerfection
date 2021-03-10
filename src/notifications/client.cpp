#include "client.h"

int main(int argc, char *argv[]) {
  std::cout << "Starting Pi Client.";

  std::string msg;
  std::getline(std::cin, msg);
  int port = 8080;
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  int addrlen = sizeof(address);

  int server_connect = inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  // Notify::err_msg(server_connect, "server_address");

  int connection =
      connect(client_fd, (struct sockaddr *)&address, sizeof(address));
  // Notify::err_msg(connection, "server_connect");
  int sent = 0;
  int total = msg.length();
  printf("%s size of %d \n", msg.c_str(), total);
  while (sent < total) {
    int s = send(client_fd, msg.substr(sent).c_str(), total, 0);
    printf("%s \n", msg.substr(sent).c_str());
    sent = sent + s;
    printf("%d \n ", sent);
  };

  // int shut = shutdown(client_fd, SHUT_RDWR);
  // Notify::err_msg(shut, "socket_shutdown");
  // int cls = close(client_fd);
  // Notify::err_msg(cls, "socket_close");
  // return 0;
}