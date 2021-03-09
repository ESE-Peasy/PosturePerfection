#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <errno.h>

using namespace std;

int err_msg (int num, string msg){
    if (num < 0 ){ 
        cerr << "Error with "<< msg << "\n" << "Error Code:"<< errno << "\n"; 
        exit(1);
        }
    return 0;
}

int main(int argc, char *argv[])
{
    cout << "Starting server \n";
    int port = 8080;
    int server_fd = socket(AF_INET,SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    int opt = 1;
    int setup = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) ;

    int addrlen = sizeof(address);
    int binding  = bind(server_fd, (struct sockaddr *)&address,  addrlen);
    int listening = listen(server_fd, 5);
    int pi_fd = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen);

    char buffer[1024] ;
    int posture_value = read(pi_fd, buffer, 1024); 
    string buff (buffer);
    string start ("notify-send \"Posture Perfection\" \"");
    string end ("\" -u critical --icon /$HOME/code/Uni/Posture/PosturePerfection/docs/images/posture-logo-no-text.png");
    string out = start + buff + end;
    system(out.c_str());
    cout << buff; 
    shutdown(server_fd, SHUT_RDWR);
    shutdown(pi_fd, SHUT_RDWR);
    close(server_fd);
    close(pi_fd);
    return 0;
}
