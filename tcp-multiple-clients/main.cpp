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
        error_exit("[103]Bind failed.", 103);



    // listen()
    int listen_result = listen(socket_fd, SOMAXCONN);
    if (listen_result < 0)
        error_exit("[104]Listen failed.", 104);



    // Set sockaddr_in for client info and accept()
    sockaddr_in client_info;
    socklen_t  client_infoSize = sizeof(client_info);
    char        host[NI_MAXHOST];
    char        service[NI_MAXSERV];
    memset((void*)host, 0, NI_MAXHOST);       //Cleaning buffer, maybe garbage in there.
    memset((void*)service, 0, NI_MAXSERV); //Cleaning...
    int client_socket = accept(socket_fd, (sockaddr*)&client_info, &client_infoSize);

    if (client_socket < 0)
    {
        close (socket_fd);
        error_exit("[105]Accept() failed.", 105);
    }
    //if (!(getaddrinfo(&host, &service, &client_info, &client_infoSize))
    //{
    //    close(socket_fd);
    //    close(client_socket);
    //    error_exit("[106]Failed for fetch client addr info.", 106);
    //}





    // Don't know if close of socket_fd should be actually done here if it's non-blocking, probably not.
    // Close listen() step.. (?)
    close(socket_fd);

    // Practice of extracting data from struct.. just prints address of client.
    std::cout << "Accepted connection from: " << inet_ntoa(client_info.sin_addr) << std::endl;

    char buff[4096];
    while(true)
    {
        // Clear buffer.
        memset((void*)buff, 0, 4096);

        // Wait for msg
        int bytesReceived = recv(client_socket, &buff, 4096, 0);
        if (bytesReceived < 0)
        {
            close(client_socket);
            error_exit("[107]Connection error when receiving bytes.", 107);
        }
        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected.\n";
            break ;
        }

        // Display msg
        std::cout << "Received this string: " << std::string(buff, 0, bytesReceived) << std::endl;

        //Resend same msg back to client
        send(client_socket, &buff, bytesReceived + 1, 0);
    }



    // Done.
    close(client_socket);

    return (0);
}