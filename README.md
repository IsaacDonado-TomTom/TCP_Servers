# Table of Contents
1. [Making a simple TCP server without non-blocking abilities](#tcp_server)


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


