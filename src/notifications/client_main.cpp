#include "client.h"

int main(int argc, char *argv[]) {
  char *ip = (char *)"127.0.0.1";
  Notify::NotifyClient client(ip, 8080);
  std::string msg("Hello World");
  client.sendMessage(msg);
  return 1;
}