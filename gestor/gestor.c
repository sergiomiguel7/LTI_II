// Server side implementation of TCP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "../concentrador/server_api.h"
#define SA struct sockaddr
#define PORT_TCP 7778
#define MAXLINE 1024
#define MAX 128

int fd;

void *func(void *arg);

int main()
{
    int sockfd, connfd, client_sock, len;
    struct sockaddr_in servaddr, cli;
    pthread_t tid;

    fd = open("../db/data.txt", O_RDWR | O_CREAT | O_APPEND, 0666);

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT_TCP);

    // Binding created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    len = sizeof(struct sockaddr);

    // Accept the data packet from client and verification
    while ((client_sock = accept(sockfd, (SA *)&cli, &len)))
    {
        if (client_sock < 0)
        {
            printf("server acccept failed...\n");
        }
        else
            printf("server acccept the client...\n");

        if (pthread_create(&tid, NULL, func, (void *)&client_sock))
        {
            perror("not created");
            return 1;
        }
    }

    // After chatting close the socket
    close(fd);
}

void *func(void *arg)
{
    int sock = *(int *)arg;
    int readSize = 0;
    int started = 0;
    char buff[MAX];

    while (1)
    {

        // read the message from client and copy it in buffer
        while ((readSize = recv(sock, buff, sizeof(buff), 0)) > 0)
        {
            if (started == 0)
            {
                char password[8];
                strncpy(password, &buff[2], 7);
                password[7] = '\0';
                started = 1;
                if (strncmp(password, "G1MIETI", 7) == 0)
                {
                    send(sock, "ok", 3, 0);
                }
                else
                {
                    char error[3];
                    error[0] = END_TCP;
                    error[1] = AUTH_FAIL;
                    error[2] = '\0';
                    send(sock, error, 3, 0);
                    shutdown(sock, SHUT_RDWR);
                    return 0;
                }
                bzero(buff, MAX);
            }
            write(fd, buff, strlen(buff));
            bzero(buff, MAX);
        }

        if (readSize == 0)
        {
            printf("Client disconnect\n");
            fflush(stdout);
        }
        else if (readSize == -1)
        {
            perror("recv failed");
        }
        return 0;

        // if msg contains "Exit" then server exit and chat ended(needs to change)
        /* if (strncmp("exit", buff, 4) == 0)
        {
            printf("Server Exit...\n");
            break;
        } */
    }
}