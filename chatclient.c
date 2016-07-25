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

// sets up the connection from this client to server
int connectToServer(struct hostent *server, int portno){

    //create new socket
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
        return -1;
    }

    struct sockaddr_in serv_addr;

    // clear memory of the struct
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // build the struct
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    // attempt to connect to server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
        return -1;
    }
    return sockfd;
}




// get users chat handle. Can be up to 10 chars long + \n + \0. with 500 char limit on message
// we only need a 512 byte block of memory to send entire handle and message
void createHandle(char handle[12]) {

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
    // adds the '>' and the ' ' chars to handle
    handle[strlen(handle)] = '>';
    handle[strlen(handle)] = ' ';
}

void sendInitialMessage(int socketfd, char handle[12], char* port) {
    int n;
    char message[BUFFER_SIZE];
    memset(message,'\0', BUFFER_SIZE);
    strcpy(message, handle);
    strcat(message, port);
    n = write(socketfd, &message, strlen(message));
    if (n < 0) {
        error("ERROR sending message\n");
    }

}

int sendMessage (int socketfd, char handle[12]) {
    int n;
    //setup and clear message space
    char message[BUFFER_SIZE];
    memset(message,'\0', BUFFER_SIZE);
    strcpy(message, handle);

    char messageBuffer[MESSAGE_SIZE];
    memset(messageBuffer,'\0', BUFFER_SIZE);


    // prompt for message
    printf(handle);
    if ((fgets(messageBuffer, MESSAGE_SIZE-1, stdin) == NULL)) {
        fprintf(stderr, "Error reading message.\n");
        return -1;
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
            return -1;
        }
        printf("Chat session ended\n");
        close(socketfd);
        return -2;

    }
        // if quit message is not found, concat the handle and the message and send to server
    else {
        strcat(message, messageBuffer);
        n = write(socketfd, &message, strlen(message));
        if (n < 0) {
            error("ERROR sending message\n");
            return -1;
        }
    }

    return 0;
}

// read message from socketfd
int readMessage(int socketfd) {
    int n;
    char message[MESSAGE_SIZE];
    memset(message,'\0', MESSAGE_SIZE);

    n = read(socketfd, message, BUFFER_SIZE-1);
    if (n < 0) {
        error("ERROR reading from the socket\n");
    }
    printf("%s\n", message);

    if(strstr(message, "\\quit") != NULL) {
        printf("Server has closed the connection\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int portno;
    struct hostent *server;

    // check that we have the right amount of arguments
    if (argc < 3) {
        fprintf(stderr,"USAGE %s hostname port\n", argv[0]);
        exit(0);
    }

    // get server name
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // get port number
    portno = atoi(argv[2]);

    // attempt to connect client to server
    int sockfd = connectToServer(server, portno);

    // create the users chat handle
    char handle[12];
    createHandle(handle);

    // send the initial communication message with handle and port
    sendInitialMessage(sockfd, handle, argv[2]);

    int chat;
    chat = 0;

    // send and receive messages until 'quit' is entered
    while(chat == 0){
        chat = readMessage(sockfd);
        chat = sendMessage(sockfd, handle);
    }

    close(sockfd);
    return 0;
}