// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "api.h"  
#define SA struct sockaddr
#define PORT_UDP    7777
#define PORT_TCP    7778
#define MAXLINE 1024
#define MAX 80
struct sockaddr_in cliaddr_TCP;
struct sockaddr_in servaddr_UDP;
struct sockadrr_in cli;

void func(int sockfd_TCP, int sockfd_UDP);
  
  
//servidor UDP e cliente TCP
int main() {
    int sockfd_UDP;
    int sockfd_TCP;
    int connfd;
    char buffer[MAXLINE];
    char message[MAXLINE];
      
    // Creating socket file descriptor
    if ( (sockfd_UDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr_UDP, 0, sizeof(servaddr_UDP));
    memset(&cliaddr_TCP, 0, sizeof(cliaddr_TCP));
      
    // Filling server information
    servaddr_UDP.sin_family    = AF_INET; // IPv4
    servaddr_UDP.sin_addr.s_addr = INADDR_ANY;
    servaddr_UDP.sin_port = htons(PORT_UDP);
      
    // Bind the socket with the server address
    if ( bind(sockfd_UDP, (const struct sockaddr *)&servaddr_UDP, 
            sizeof(servaddr_UDP)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
      
    int len, n;
  
    len = sizeof(servaddr_UDP);  //len is value/resuslt
  
    n = recvfrom(sockfd_UDP, (char *)buffer, MAXLINE, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr_TCP,
                &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    memset(message, 0, sizeof message);
    sprintf(message,"1;%s",buffer);        //print ID do concentrador

    //escrever TCP a message
    // socket create and varification
    sockfd_TCP = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_TCP == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&cliaddr_TCP, sizeof(cliaddr_TCP));

    // assign IP, PORT
    cliaddr_TCP.sin_family = AF_INET;
    cliaddr_TCP.sin_addr.s_addr = inet_addr("127.0.0.1");
    cliaddr_TCP.sin_port = htons(PORT_TCP);
  
    // connect the client socket to server socket
    if (connect(sockfd_TCP, (SA*)&cliaddr_TCP, sizeof(cliaddr_TCP)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
  
    // function for chat
    func(sockfd_TCP, sockfd_UDP);
  
    // close the socket
    close(sockfd_TCP);  
      
    return 0;
}

void func(int sockfd_TCP, int sockfd_UDP)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
  
        // read the message from client and copy it in buffer
        read(sockfd_UDP, buff, sizeof(buff));
        // print buffer which contains the UDP Server
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;
  
        // and send that buffer to TCP Server
        write(sockfd_TCP, buff, sizeof(buff));
  
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}