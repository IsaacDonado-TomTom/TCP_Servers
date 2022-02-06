# Table of Contents
1. [Making a simple TCP server without non-blocking abilities](#tcp_server)
  + [Allocating a socket](#socket)
  + [Bind socket to port](#bind)


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



