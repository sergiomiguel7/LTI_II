// Server side implementation of UDP client-server model and TCP Client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define SA struct sockaddr
#define PORT_UDP    7777
#define PORT_TCP    7778
#define MAXLINE 1024
#define MAX 128

struct sockaddr_in cliaddr_TCP, servaddr_UDP;
void func(int sockfd_TCP, int sockfd_UDP);
void checkHostName(int hostname);
void checkHostEntry(struct hostent * hostentry);
void checkIPbuffer(char *IPbuffer);

char hostbuffer[256];
char *IPbuffer;
struct hostent *host_entry;
int hostname;
  
  

int main() {
    int sockfd_UDP;
    int sockfd_TCP;
    int connfd;
    char buffer[MAXLINE];
    char message[MAXLINE];

      
    //UDP SERVER SIDE  
    // Creating socket file descriptor
    if ( (sockfd_UDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("\nsocket creation failed");
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
        perror("\nbind failed on server");
        _exit(1);
    }
      

    //TCP CLIENT SIDE  
    // socket create and varification
    sockfd_TCP = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_TCP == -1) {
        printf("\nsocket tcp creation failed...");
        exit(0);
    }

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
  
    // function for chat
    func(sockfd_TCP, sockfd_UDP);
  
    // close the socket
    close(sockfd_TCP);  
      
    return 0;
}

void func(int sockfd_TCP, int sockfd_UDP)
{
    char buff[MAX];
    char message[MAX];

     // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
  
    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);
  
    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
    
    // infinite loop for chat
    while (1) {
        bzero(buff, MAX);
        bzero(message, MAX);
  
        // read the message from UDP server and copy it into the TCP client
        read(sockfd_UDP, buff, sizeof(buff));
        sprintf(message,"%s;%s",IPbuffer,buff);

        // print buffer's content from the the TCP client
        //printf("To server: %s\n", message);
        // copy server message in the buffer
  
        // and send that buffer to TCP Server
        write(sockfd_TCP, message, strlen(message));
  
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
        usleep(100000);
    }

}

// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}
  
// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}
  
// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}