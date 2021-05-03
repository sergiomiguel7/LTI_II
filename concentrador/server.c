// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "api.h"  

#define PORT_UDP    7777
#define PORT_TCP    7778
#define MAXLINE 1024
struct sockaddr_in cliaddr_TCP;
struct sockaddr_in servaddr_UDP;
  
  
//servidor UDP e cliente TCP
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char message[MAXLINE];
      
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
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
    if ( bind(sockfd, (const struct sockaddr *)&servaddr_UDP, 
            sizeof(servaddr_UDP)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
      
    int len, n;
  
    len = sizeof(cliaddr_TCP);  //len is value/resuslt
  
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr_TCP,
                &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    memset(message, 0, sizeof message);
    sprintf(message,"1;%s",buffer);

    //escrever TCP a message


    printf("Hello message sent.\n"); 
      
    return 0;
}