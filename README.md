*Credits to: [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/) & [Sloan Kelly from YouTube](https://www.youtube.com/channel/UC4LMPKWdhfFlJrJ1BHmRhMQ), most of the stuff written on this document has been extracted and slightly simplified or modified from those two sources*

# Table of Contents
1. [Making a simple TCP server without non-blocking abilities](#tcp_server)
  + [Allocating a socket](#socket)
  + [Bind socket to port](#bind)
  + [Listen for incomming connections](#listen)
  + [Accept pending connection](#accept)
  + [Receive data on a socket](#recv)



<a href name="tcp_server"></a>
## Making a simple TCP server without non-blocking abilities

The steps for a server to listen for a conenction are the following
```text
1. Create a socket
2. Bind the socket
3. Mark port for listening
4. Accept a call
5. Close the listening socket
6. While receiving, do something
7. Close the socket
```

<a name="socket"></a>
# int socket(int domain, int type, int protocol)
To Allocate a socket descriptor.

```text
#include <sys/types.h>
#include <sys/socket.h>
```

**Parameters**
+ **domain** describes what kind of socket you’re interested in. This can, believe me, be a wide variety of things, but since this is a socket guide, it’s going to be PF_INET for IPv4, and PF_INET6 for IPv6.
+ **type** parameter can be a number of things, but you’ll probably be setting it to either SOCK_STREAM for reliable TCP sockets (send(), recv()) or SOCK_DGRAM for unreliable fast UDP sockets (sendto(), recvfrom()). (Another interesting socket type is SOCK_RAW which can be used to construct packets by hand. It’s pretty cool.)
+ **protocol** parameter tells which protocol to use with a certain socket type. Like I’ve already said, for instance, SOCK_STREAM uses TCP. Fortunately for you, when using SOCK_STREAM or SOCK_DGRAM, you can just set the protocol to 0, and it’ll use the proper protocol automatically. Otherwise, you can use getprotobyname() to look up the proper protocol number.

**Description**
Returns a new socket descriptor that you can use to do sockety things with. This is generally the first call in the whopping process of writing a socket program, and you can use the result for subsequent calls to listen(), bind(), accept(), or a variety of other functions.
In usual usage, you get the values for these parameters from a call to getaddrinfo(), as shown in the example below. But you can fill them in by hand if you really want to.

**Return**
The new socket descriptor to be used in subsequent calls, or -1 on error (and errno will be set accordingly).
```text
Useful definitions: 
PF_NET - In practice, the same as AF_NET but the standard is to call PF_NET from socket and AF_NET from the sockaddre struct. This stands for net Address Family and refers to Pv4 IP addresses, AF_NET6 refers to Pv6 addresses for instance.
SOCKSTREAM - Represents that we're dealing with reliable TCP socket. SOCK_DGRAM for example refers to a UDP socket.
```



<a name="bind"></a>
# int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen)
Associate a socket with an IP address and port number

```text
#include <sys/types.h>
#include <sys/socket.h>
```

**Parameters**
+ **sockfd** This is the socket fd you want to bind with the IP & Port, the return value of socket()
+ **sockaddr \*my_addr** You must declare a sockaddr_in struct and set the sin_family, port and specify which addresses will be allowed to connect and pass it as an arguement
+ **addrlen** The result of sizeof of the struct you set for the previous parameter. 

**Description**
When a remote machine wants to connect to your server program, it needs two pieces of information: the IP address and the port number. The bind() call allows you to do just that.

First, you call getaddrinfo() to load up a struct sockaddr with the destination address and port information. Then you call socket() to get a socket descriptor, and then you pass the socket and address into bind(), and the IP address and port are magically (using actual magic) bound to the socket!

If you don’t know your IP address, or you know you only have one IP address on the machine, or you don’t care which of the machine’s IP addresses is used, you can simply pass the AI_PASSIVE flag in the hints parameter to getaddrinfo(). What this does is fill in the IP address part of the struct sockaddr with a special value that tells bind() that it should automatically fill in this host’s IP address.

What what? What special value is loaded into the struct sockaddr’s IP address to cause it to auto-fill the address with the current host? I’ll tell you, but keep in mind this is only if you’re filling out the struct sockaddr by hand; if not, use the results from getaddrinfo(), as per above. In IPv4, the sin_addr.s_addr field of the struct sockaddr_in structure is set to INADDR_ANY. In IPv6, the sin6_addr field of the struct sockaddr_in6 structure is assigned into from the global variable in6addr_any. Or, if you’re declaring a new struct in6_addr, you can initialize it to IN6ADDR_ANY_INIT.

Lastly, the addrlen parameter should be set to sizeof my_addr.

**Return**
Returns zero on success, or -1 on error (and errno will be set accordingly).
```text
Useful definitions: 
AF_NET - his stands for net Address Family and refers to Pv4 IP addresses, AF_NET6 refers to Pv6 addresses for instance.
INADDDRANY - When we don't know the IP address of our machine, we can use the special IP address INADDR_ANY. It allows our server to receive packets that have been targeted by any of the interfaces. This is an IP address that is used when we don't want to bind a socket to any specific IP.
```





<a name="listen"></a>
# int listen(int sockfd, int backlog)
Tell a socket to listen for incoming connections

```text
#include <sys/socket.h>
```

**Parameters**
+ **sockfd** This is the socket fd you want to listen through, the return value of socket()
+ **backlog** The backlog parameter can mean a couple different things depending on the system you on, but loosely it is how many pending connections you can have before the kernel starts rejecting new ones. So as the new connections come in, you should be quick to accept() them so that the backlog doesn’t fill. Try setting it to 10 or so, and if your clients start getting “Connection refused” under heavy load, set it higher.

**Description**
You can take your socket descriptor (made with the socket() system call) and tell it to listen for incoming connections. This is what differentiates the servers from the clients, guys.
Before calling listen(), your server should call bind() to attach itself to a specific port number. That port number (on the server’s IP address) will be the one that clients connect to.

**Return**
Returns zero on success, or -1 on error (and errno will be set accordingly).
```text
Useful definitions: 
SOMAXCONN - Socket maximum connection number established by the OS.
```




<a name="accept"></a>
# int accept(int sockfd, struct sockaddr* storage, socklen_t* addrlen)
Accept an incoming connection on a listening socket

```text
#include <sys/types.h>
#include <sys/socket.h>
```

**Parameters**
+ **sockfd** The socket fd used in the listen() function, usually result of socket()
+ **storage** A pointer to a local struct that will be filled with information about the connecting client.
+ **addrlen** A pointer to the result of sizeof of the previous parameter.

**Description**
Once you’ve gone through the trouble of getting a SOCK_STREAM socket and setting it up for incoming connections with listen(), then you call accept() to actually get yourself a new socket descriptor to use for subsequent communication with the newly connected client.

The old socket that you are using for listening is still there, and will be used for further accept() calls as they come in.

accept() will normally block, and you can use select() to peek on the listening socket descriptor ahead of time to see if it’s “ready to read”. If so, then there’s a new connection waiting to be accept()ed! Yay! Alternatively, you could set the O_NONBLOCK flag on the listening socket using fcntl(), and then it will never block, choosing instead to return -1 with errno set to EWOULDBLOCK.

The socket descriptor returned by accept() is a bona fide socket descriptor, open and connected to the remote host. You have to close() it when you’re done with it.

**Return**
accept() returns the newly connected socket descriptor, or -1 on error, with errno set appropriately.
```text
Useful definitions: 
NI_MAXHOST - Reasonable max host buffer size. 1025
NI_MAXSERV - Reasonable max service buffer size. 32
```





<a name="recv"></a>
# ssize_t recv(int sockfd, void* buff, size_t len, int flags)
Receive data on a socket

```text
#include <sys/types.h>
#include <sys/socket.h>
```

**Parameters** 
+ **sockfd** the socket descriptor you want to send data to (whether it’s the one returned by socket() or the one you got with accept()), in our case it's the one from accept().
+ **buff** buff is a pointer to the data you want to send
+ **len** len is the length of that data in bytes
+ **flags** Just set flags to 0.

**Description**
Once you have a socket up and connected, you can read incoming data from the remote side using the recv().
When you call recv(), it will block until there is some data to read. If you want to not block, set the socket to non-blocking or check with select() or poll() to see if there is incoming data before calling recv() or recvfrom().

**Return**
Returns the number of bytes actually received (which might be less than you requested in the len parameter), or -1 on error (and errno will be set accordingly).

If the remote side has closed the connection, recv() will return 0. This is the normal method for determining if the remote side has closed the connection. Normality is good, rebel!
```text
Useful definitions for flags: 
MSG_OOB - Receive Out of Band data. This is how to get data that has been sent to you with the MSG_OOB flag in send(). As the receiving side, you will have had signal SIGURG raised telling you there is urgent data. In your handler for that signal, you could call recv() with this MSG_OOB flag.
MSG_PEEK - If you want to call recv() “just for pretend”, you can call it with this flag. This will tell you what’s waiting in the buffer for when you call recv() “for real” (i.e. without the MSG_PEEK flag. It’s like a sneak preview into the next recv() call.
MSG_WAITALL - Tell recv() to not return until all the data you specified in the len parameter. It will ignore your wishes in extreme circumstances, however, like if a signal interrupts the call or if some error occurs or if the remote side closes the connection, etc. Don’t be mad with it.
```
