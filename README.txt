CS 372 – Project 1 – TCP Chat
Jaden Yuros

Testing was completed on flip1, flip2, and flip3 servers.

Instructions to run:

1) run command 'make' to compile the chatclient.c file
2) run command 'python chatserve.py port' where port is the port # you wish to
    start the server on.
3) run command 'chatclient hostname port' where hostname is the server that
    chatserve is currently run on, and port is the port # that chatserve is
    listening on
4) At this point the client and server should be connected and the client
    should have sent the initial message with the client handle and port
    and is now waiting for the server to sent the first message back to client
5) Send message from server to client
6) Send message from client to server
7) type '\quit' into either client or server when you wish to end the connection
8) run command 'make clean' to clean the directory of chatclient
