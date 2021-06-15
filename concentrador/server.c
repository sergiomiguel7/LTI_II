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
#include "server_api.h"

struct sockaddr_in cliaddr_TCP, servaddr_UDP;
void func(int sockfd_TCP, int sockfd_UDP);

char hostbuffer[256];
char *IPbuffer;
char area[12];
char user[14];
struct hostent *host_entry;
int hostname;
int ID;

int main(int argc, char* argv[])
{

    strcpy(area, argv[1]);
    ID = atoi(argv[2]);
    strcpy(user, argv[3]);
    int sockfd_UDP;
    int sockfd_TCP;
    int connfd;
    char buffer[MAXLINE];
    char message[MAXLINE];

    //UDP SERVER SIDE
    // Creating socket file descriptor
    if ((sockfd_UDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("\nsocket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr_UDP, 0, sizeof(servaddr_UDP));
    memset(&cliaddr_TCP, 0, sizeof(cliaddr_TCP));

    // Filling server information
    servaddr_UDP.sin_family = AF_INET; // IPv4
    servaddr_UDP.sin_addr.s_addr = INADDR_ANY;
    servaddr_UDP.sin_port = htons(PORT_UDP);

    // Bind the socket with the server address
    if (bind(sockfd_UDP, (const struct sockaddr *)&servaddr_UDP,
             sizeof(servaddr_UDP)) < 0)
    {
        perror("\nbind failed on server");
        _exit(1);
    }

    //TCP CLIENT SIDE
    // socket create and varification
    sockfd_TCP = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_TCP == -1)
    {
        printf("\nsocket tcp creation failed...");
        exit(0);
    }

    bzero(&cliaddr_TCP, sizeof(cliaddr_TCP));

    // assign IP, PORT
    cliaddr_TCP.sin_family = AF_INET;
    cliaddr_TCP.sin_addr.s_addr = inet_addr("127.0.0.1");
    cliaddr_TCP.sin_port = htons(PORT_TCP);

    // connect the client socket to server socket
    if (connect(sockfd_TCP, (SA *)&cliaddr_TCP, sizeof(cliaddr_TCP)) != 0)
    {
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
    char message[MAX2];

    //auth on gestor
    bzero(buff, MAX);
    bzero(message, MAX2);

    char start[9];
    start[0] = START_TCP;
    start[1] = ID;
    char *auth = "G1MIETI";
    sprintf(start, "%d%d%s", START_TCP, ID, auth);

    write(sockfd_TCP, start, strlen(start));

    while ((recv(sockfd_TCP, buff, sizeof(buff), 0) > 0))
    {
        if (buff[0] == END_TCP)
        {
            write(1, "\nauth failed\n", 14);
            _exit(1);
        }

        if (strncmp(buff, "ok", 2) == 0)
        {
            write(1, "\nauth granted\n", 14);
            break;
        }
    }

    // infinite loop for chat
    while (1)
    {
        bzero(buff, MAX);
        bzero(message, MAX2);

        // read the message from UDP server and copy it into the TCP client
        read(sockfd_UDP, buff, sizeof(buff));
        sprintf(message, "%s;%s;%d;%s",user,area,ID,buff);

        // and send that buffer to TCP Server
        write(sockfd_TCP, message, strlen(message));

        usleep(100000);
    }
}
