#include <iostream>
#include <stdlib.h> // For atoi()
#include <sys/types.h> // For socket()
#include <sys/socket.h> // For socket()
#include <string.h> // For memset()
#include <unistd.h> // For close()
#include <arpa/inet.h> // for htons()
#include <netdb.h>  // Definitions for default buffer sizes.
#include <sys/select.h> // For select()

int         server_port;
std::string server_password;

void    error_exit(const std::string& msg, int exit_code)
{
    std::cerr << msg << std::endl;
    exit(exit_code);
}

int main(int argc, char** argv)
{
    if (argc != 3)
        error_exit("[101]Wrong number of arguements.", 101);

    server_port = atoi(argv[1]);
    server_password = std::string(argv[2]);
    std::cout << "Port: " << server_port << std::endl << "Password: " << server_password << std::endl;




    // Create a Socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        error_exit("[102]socket() call for socket_fd failed", 102);


    
    // Set sockaddr_in struct and bind()
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(server_port); 
    hint.sin_addr.s_addr = INADDR_ANY;
    int bind_result = bind(socket_fd, (sockaddr*)&hint, sizeof(hint));
    if (bind_result < 0)
    {
        close(socket_fd);
        error_exit("[103]Bind failed.", 103);
    }



    // listen()
    int listen_result = listen(socket_fd, SOMAXCONN);
    if (listen_result < 0)
    {
        close(socket_fd);
        error_exit("[104]Listen failed.", 104);
    }

    // Select magic
    fd_set  master;
    fd_set  copy;
    int     socket_count;
    FD_ZERO(&master);           // Initialized "master" set and cleared it for safety. 
    FD_SET(socket_fd, &master); // Add main socket to the set. (?)
    while(true)
    {
        // Copy since select destroys "unneeded" info from struct
        copy = master; 
    }


    close(socket_fd);
    return (0);
}