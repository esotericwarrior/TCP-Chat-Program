/***************************************************************************
 *                                                                         *
 *      Class:          CS 372 - Introduction to Networks                  *
 *      Date:           07/22/2016                                         *
 *      Description:    Project 1 - TCP Chat Client                        *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 512
#define MESSAGE_SIZE 512

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // check that we have the right amount of arguments
    char buffer[BUFFER_SIZE];
    if (argc > 3) {
        printf("usage error, try: %s hostname port\n", argv[0]);
    }

    //create new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr,"ERROR: Could not open socket\n");
        exit(1);
    }

    // setup server
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("ERROR: no such host\n");
    }

    // clear memory and get port number
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);

    // build the struct
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    // Attempt to bind socket with port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR: on binding\n");
    }

    // get message from the user
    printf("Enter the message:  ");
    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE-1, stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR: writing to the socket\n");
    }
    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE-1);
    if (n < 0) {
        error("ERROR: reading from the socket\n");
    }
    printf("MSG:\t%s\n", buffer);
    close(sockfd);
    return 0;

}