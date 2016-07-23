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
#define MESSAGE_SIZE 500

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int sendMessage (int socketfd, char handle[12]) {
    int n;
    char message[BUFFER_SIZE];
    memset(message,'\0', BUFFER_SIZE);
    strcpy(message, handle);

    char messageBuffer[MESSAGE_SIZE];
    memset(messageBuffer,'\0', BUFFER_SIZE);


    // prompt for message
    printf(handle);
    if ((fgets(messageBuffer, MESSAGE_SIZE-1, stdin) == NULL)) {
        fprintf(stderr, "Error reading message.\n");
        exit(1);
    }
    // trims off the newline char from input
    messageBuffer[strcspn(messageBuffer, "\n")] = 0;

    // clears the input stream if there are extra characters entered
    if(strlen(messageBuffer) > MESSAGE_SIZE) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF );
    }

    // checks for \quit message
    if(strncmp(messageBuffer, "\\quit", 4) == 0) {
        n = write(socketfd, &messageBuffer, strlen(messageBuffer));
        if (n < 0) {
            error("ERROR sending message\n");
            exit(1);
        }
        printf("Chat session ended\n");
        close(socketfd);
        exit(0);

    }

    else {
        strcat(message, messageBuffer);
        n = write(socketfd, &message, strlen(message));
        if (n < 0) {
            error("ERROR sending message\n");
            exit(1);
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // check that we have the right amount of arguments
    char buffer[BUFFER_SIZE];
    if (argc < 3) {
        fprintf(stderr,"USAGE %s hostname port\n", argv[0]);
        exit(0);
    }

    // get port number
    portno = atoi(argv[2]);

    //create new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // setup server
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // clear memory of the struct
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // build the struct
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    // attempt to connect to server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");





    // get users chat handle. Can be up to 10 chars long + \n + \0. with 500 char limit on message
    // we only need a 512 byte block of memory to send entire handle and message
    char handle[12];
    memset(handle,'\0', 12);
    printf("Please enter your chat handle(up to 10 characters):  ");
    if ((fgets(handle, 11, stdin) == NULL)) {
        fprintf(stderr, "Error reading handle.\n");
        exit(1);
    }
    // trims off the newline char from input
    handle[strcspn(handle, "\n")] = 0;

    // clears the input stream if there are extra characters entered
    if(strlen(handle) > 9) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF );
    }
    // adds the '>' char to handle
    handle[strlen(handle)] = '>';

    // send sever the port number
    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, handle);
    strcat(buffer, argv[2]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing to the socket\n");
        close(sockfd);
        exit(1);
    }





    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE-1);
    if (n < 0) {
        error("ERROR reading from the socket\n");
    }
    printf("MSG:\t%s\n", buffer);
    close(sockfd);
    return 0;

}