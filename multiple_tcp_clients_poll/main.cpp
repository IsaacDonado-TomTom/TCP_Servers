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
#include <fcntl.h>
#include <vector>
#include <poll.h> // for poll()

int         server_port;
std::string server_password;

void    error_exit(const std::string& msg, int exit_code)
{
    std::cerr << msg << std::endl;
    exit(exit_code);
}

int main(int argc, char** argv)
{
    if (argc != 2)
        error_exit("[101]Usage: ./a.out [PORT_NUMBER]", 101);

    server_port = atoi(argv[1]);
    std::cout << "Port: " << server_port << std::endl;




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



    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    ////////////////////////////////////////////////////////////////////////////////////
    //     poll() magic                   poll() magic               poll() magic     //
    ////////////////////////////////////////////////////////////////////////////////////
    nfds_t  nfds = 0;
    struct pollfd *pollfds;
    int numfds = 0;
    int maxfds = 0;

    // malloc() pollfds struct pointer
    int max_clients = 5;
    pollfds = (pollfd*)malloc(max_clients * sizeof(struct pollfd)); // max_clients is defined above,  you have to make the struct bigget later if necessary.
    if (pollfds == NULL)
    {
        close(socket_fd);
        error_exit("[105]pollfds malloc() failed.", 105);
    }
    maxfds = max_clients;

    pollfds->fd = socket_fd; // We set the fd in the pollfds struct to our listening socket_fd.
    pollfds->events = POLLIN; // we're interested if Input operations are possible, POLLIN for that.
    numfds = 1;

    // declare client addr structs
    socklen_t                   client_addrlen;
    struct  sockaddr_storage    client_socket_addr;
    struct  sockaddr_in         *ptr_client_sockaddr;
    struct  tnode               *root = NULL;

    

    int new_fd; // used as return value when accepting a new connection.
    char    buf[4096];
    ssize_t bytes_received;
    while (true)
    {
        nfds = numfds;


        // poll() call to see what's available to read
        if (poll(pollfds, nfds, 0) == -1) // the zero is to set it to non-blocking.
        {
            free(pollfds);
            close(socket_fd);
            error_exit("[106]poll() call failed.", 106);
        }

        // "Loop" through pollfds structs and see what's ready
        for (int fd = 0; fd < (nfds+1); fd++)
        {
            if ((pollfds + fd)->fd <= 0) // fd shouldn't ever be 0(stdin)..
                continue ;

            if (((pollfds + fd)->revents & POLLIN) == POLLIN)   // whatever struct pollfds+fd is, it's ready for reading
            {
                // Check if the socket ready for reading is the listening socket (new connection to be accept()ed).
                if ((pollfds + fd)->fd == socket_fd)
                {
                    // New connection struct magic.
                    client_addrlen = sizeof(struct sockaddr_storage);
                    if ( (new_fd = accept(socket_fd, (struct sockaddr*)&client_socket_addr, &client_addrlen)) ==  -1)
                    {
                        free(pollfds);
                        close(socket_fd);
                        error_exit("[107]failed accept() call when new connection found.", 107);
                    }

                    // If numfds == maxfds then make more space in pollfds
                    if (maxfds == numfds)
                    {
                        pollfds = (pollfd*)realloc(pollfds, (maxfds + max_clients) * sizeof(struct pollfd));
                        if (pollfds == NULL)
                        {
                            close(socket_fd);
                            error_exit("[105]pollfds realloc() failed.", 105);
                        }
                        maxfds += max_clients;
                    }

                    numfds++; // One more client about to be connected.
                    (pollfds + numfds - 1)->fd = new_fd;
                    (pollfds + numfds - 1)->events = POLLIN;
                    (pollfds + numfds - 1)->revents = 0;
                    ptr_client_sockaddr = (sockaddr_in*)&client_socket_addr;
                    std::cout << "Accepted new connection from: " << inet_ntoa(ptr_client_sockaddr->sin_addr) << std::endl;
                    

                }
                else    // Data from existing connection.
                {
                    memset((void*)&buf, 0, 4096);
                    bytes_received = recv((pollfds + fd)->fd, (void*)buf, 4096, 0);
                    if (bytes_received < 0)
                    {
                        std::cout << "Error receiving data from established connection.\n";
                    }
                    else if (bytes_received == 0)
                    {
                        std::cout << "Connection closed by client.\n";
                        close((pollfds+fd)->fd);
                        (pollfds+fd)->fd = -1; // Set it to be ignored in the future.
                    }
                    else
                    {
                        std::cout << "Message received: " << std::string(buf, 0, bytes_received) << std::endl;
                        send((pollfds+fd)->fd, (void*)buf, bytes_received+1, 0);
                    }
                    
                }
                
            } // End if statement checking if something is ready for reading
        } // End for loop through pollfds
    } // End infinate while loop.
    

    close(socket_fd);
    return (0);
}