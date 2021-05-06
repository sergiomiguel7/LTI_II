// Server side implementation of TCP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#define SA struct sockaddr
#define PORT_TCP 7778
#define MAXLINE 1024
#define MAX 127

struct threadParam {
    int sockfd, fd;

};

void *func(void* arg);

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    int fd = open("../db/data.txt", O_RDWR | O_CREAT | O_APPEND, 0666);

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

    len = sizeof(cli);

    // Accept the data packet from client and verification
    while (1)
    {
        pthread_t tid;
        connfd = accept(sockfd, (SA *)&cli, &len);

        if (connfd < 0)
        {
            printf("server acccept failed...\n");
        }
        else
            printf("server acccept the client...\n");
        struct threadParam param;
        param.fd=fd;
        param.sockfd=sockfd;
        pthread_create(&tid,NULL,func,&param);
    }


    // After chatting close the socket
    close(sockfd);
    close(fd);
}

void *func(void* arg)
{
    struct threadParam *param= arg;
    char buff[MAX];
    char aux[MAX];

    while (1)
    {
        bzero(buff, MAX);
        bzero(aux, MAX);

        // read the message from client and copy it in buffer
        read(param->sockfd, buff, sizeof(buff));

        // print from  the client's buffer contents
        printf("%s\n ", buff);

        //writing the content recieved from the client to the database file
        sprintf(aux, "%s\n", buff);
        write(param->fd, aux, strlen(aux));

        // if msg contains "Exit" then server exit and chat ended(needs to change)
        if (strncmp("exit", buff, 4) == 0)
        {
            printf("Server Exit...\n");
            break;
        }
    }
}