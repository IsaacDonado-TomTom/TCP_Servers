#include <iostream>
#include "ArgParser.hpp"
#include <sys/socket.h> // for socket()
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h> // for usleep()
#include <netdb.h> // for gethostbyname
#include <string.h> 

int    reconnect(int socket_fd, sockaddr_in* hint, socklen_t s_len)
{
    std::cout << "Unable to connect, Attempting to reconnect after 5 seconds...\n";
    usleep(5000000);
    int result = connect(socket_fd, (sockaddr*)hint, s_len);
    return (result);
}

int main(int argc, char** argv)
{
    ArgParser   args(argc, argv);
    // args[1] args[2] for arguements 1-2

    //Create socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        std::cout << "Client failed creating socket.\n";
        return (1);
    }

    // Create hint struct for server we're connecting to.
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(args.getPort());
    hint.sin_addr.s_addr = inet_addr(args[1].c_str());

    // Connect to server
    int connect_result = connect(socket_fd, (sockaddr*)&hint, (socklen_t)(sizeof(hint)));
    while (connect_result == -1)
        connect_result = reconnect(socket_fd, &hint, (socklen_t)(sizeof(hint)));
    

    char        buff[4096];
    int         recv_result;
    int         send_result;
    std::string str_received;
    std::string str_send;

    do
    {
        // Send something to server
        std::cout << "> ";
        getline(std::cin, str_send);
        
        send_result = send(socket_fd, str_send.c_str(), str_send.size()+1, 0);
        if (send_result == -1)
        {
            std::cout << "Failed to send data to server.\n";
            continue ;
        }

        // Receive
        memset(buff, 0, 4096);
        recv_result = recv(socket_fd, (void*)buff, 4096, 0);
        if (recv_result == -1)
        {
            std::cout << "Failed to receive data from server.\n";
            continue ;
        }
        else if (recv_result == 0)
        {
            std::cout << "Disconnected from server.\n";
            break ;
        }
        else
        {// Received from server... Define behaviour
            str_received = std::string(buff, recv_result);
            std::cout << "SERVER> " << str_received << "\r\n";
        }


    }
    while (true);
    close(socket_fd);
    return (0);
}
