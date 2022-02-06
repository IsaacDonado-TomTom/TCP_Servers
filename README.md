# Table of Contents
1. [Making a simple TCP server without non-blocking abilities](#tcp_server)
  + [Allocating a socket](#socket)


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



