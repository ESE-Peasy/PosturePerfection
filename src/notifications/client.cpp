#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

using namespace std;

int err_msg(int num, string msg) {
  if (num < 0) {
    cerr << "Error with " << msg << "\n"
         << "Error Code:" << errno << "\n";
    exit(1);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  cout << "Starting Pi Client.";
  string msg;
  getline(cin, msg);
  cout << msg;
  int port = 8080;
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  int addrlen = sizeof(address);

  int server_connect = inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  err_msg(server_connect, "server_address");

  int connection =
      connect(client_fd, (struct sockaddr *)&address, sizeof(address));
  err_msg(connection, "server_connect");

  send(client_fd, msg.c_str(), sizeof(msg.c_str()), 0);
  int shut = shutdown(client_fd, SHUT_RDWR);
  err_msg(shut, "socket_shutdown");
  int cls = close(client_fd);
  err_msg(cls, "socket_close");
  return 0;
}