#include <iostream>
#include <stdlib.h> // For atoi()
#include <sys/types.h> // For socket()
#include <sys/socket.h> // For socket()
#include <string.h> // For memset()
#include <unistd.h> // For close()
#include <arpa/inet.h> // for htons()
#include <netdb.h>  // Definitions for default buffer sizes.
#include <sys/select.h> // For select()
#include <sys/time.h> // for select

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
    int     max_fd = socket_fd;
    int     current_socket;
    int     new_client;

    // Client info struct & more
    sockaddr_in client_info;
    socklen_t  client_infoSize = sizeof(client_info);
    char        buff[4096];
    int         bytes_received;

    while(true)
    {
        // Copy since select destroys "uneeded" info from struct
        copy = master;
        socket_count = select(max_fd, &copy, NULL, NULL, NULL);
        for (int i = 0 ; i < socket_count ; i++)
        {
            current_socket = copy.fd_array[i];      // fd_array doesn't exist in fd_set in linux/macos ... Need another approach
            if (current_socket == socket_fd)
            {
                // New connection.


                // accept() new connection
                new_client = accept(socket_fd, (sockaddr*)&client_info, &client_infoSize);
                if (new_client < 0)
                {
                    close(socket_fd);
                    error_exit("[107]Error on acceptiong new connection", 107);
                }
                std::cout << "Accepted connection from: " << inet_ntoa(client_info.sin_addr) << std::endl;
                FD_SET(new_client, &master);    // Adding to master list of FDs
                if (new_client > max_fd)
                    max_fd = new_client;
            }
            else
            {
                // Message from connected client.

                // recv() actual message
                memset((void*)buff, 0, 4096);
                bytes_received = recv(current_socket, &buff, 4096, 0);
                if (bytes_received <= 0)
                {
                   std::cout << "Error or connection lost with " << inet_ntoa(client_info.sin_addr);
                   close(current_socket);
                   FD_CLR(current_socket, &master);
                }
                else
                {
                    std::cout << "[RECEIVED]: " << std::string(buff, 0, bytes_received);
                }
            }
        }
    }


    close(socket_fd);
    return (0);
}