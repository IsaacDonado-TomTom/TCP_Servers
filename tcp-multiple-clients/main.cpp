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
#include <vector>

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
    FD_ZERO(&master);           //Set fd_Set to zero.
    FD_SET(socket_fd, &master); // Add listening FD to fd_Set
    int     max_fd = socket_fd; // To keep track of highest fd for select
    std::vector<int>    fds;
    std::cout << "fds.size() : " << fds.size() << std::endl;

    std::cout << "right before while()\n";
    while (true)
    {
        fd_set  copy = master;   // make copy of master because select modifies struct.
        int     socket_count = select(max_fd + 1, &copy, NULL, NULL, NULL); // search up to max_fd, which will be changing, pass only readfds because we're receiving.


        if (FD_ISSET(socket_fd, &copy))
        {
            sockaddr_in client_info;
            socklen_t   client_infoSize = sizeof(client_info);
            int new_client = accept(socket_fd, (sockaddr*)&client_info, &client_infoSize);
            if (new_client > max_fd)
            {
                max_fd = new_client;
            }

            std::cout << "Accepted new connection from: " << inet_ntoa(client_info.sin_addr) << std::endl;
            FD_SET(new_client, &master);
            fds.push_back(new_client);
            
        }

        for(int i = 0;i < fds.size(); i++)
        {
            if (FD_ISSET(fds[i], &copy))
            {
                char    buff[4096];
                memset((void*)&buff, 0, 4096);
                int bytesReceived = recv(fds[i], (void*)buff, 4096, 0);
                if (bytesReceived <= 0)
                {
                    FD_CLR(fds[i], &master);
                    fds.erase(fds.begin() + i);
                    std::cout << "Client disconnected\n";
                }
                std::cout << "Message received: " << std::string(buff, 0, bytesReceived);
                if (std::string(buff, 0, bytesReceived).find("HOWMANY") != std::string::npos)
                {
                    for(int j = 0; j < fds.size();j++)
                    {
                        std::cout << fds[j] << "..";
                    }
                    std::cout << "\n";
                }
                send(fds[i], (void*)buff, bytesReceived+1, 0);
            }
        }

    }
    

    close(socket_fd);
    return (0);
}