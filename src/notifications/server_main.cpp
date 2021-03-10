#include "server.h"

int main(int argc, char *argv[]) {
  printf("Starting up");
  Notify::NotifyServer serv(8080);
  while (1) {
    serv.run();
  }
  return 1;
}