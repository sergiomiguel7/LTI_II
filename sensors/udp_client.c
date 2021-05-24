// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../concentrador/api.h"

#define PORT_UDP 7777
#define PORT_TCP 7778
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr;

int openFile()
{
    int fd = open("../concentrador/log/data.csv", O_RDONLY);
    if (fd < 0)
    {
        write(1, "error opening data file", 24);
        _exit(0);
    }

    return fd;
}

void handleData(int fd)
{
    int n, written = 0, prevLine = 0, skip = 0;
    char buf[1024];
    char line[1024];
    while ((n = read(fd, buf, sizeof(buf))) > 0)
    {
        char *token = strtok(buf, "\n");
        while (token != NULL)
        {
            strcpy(line, token);
            prevLine = strlen(line); //last line size
            written += prevLine;     //total writed on socket
            if ((written + prevLine) < 1024)
            {
                if (skip == 0 || skip == 3)
                {
                    skip = 0;
                    strcat(line, "\n");
                    if(strlen(line) > 39)
                        sendto(sockfd, line, strlen(line),
                           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                           sizeof(servaddr));
                    printf("line: %s", line);
                    sleep(5);
                }
                else
                    skip++;
            }
            else
            {
                prevLine = 0;
                written = 0;
                skip = 1;
            }
            memset(line, 0, strlen(line));
            token = strtok(NULL, "\n");
        }
        memset(buf, 0, strlen(buf));
    }
}

// Driver code
int main()
{

    char buffer[MAXLINE];
    int fd = openFile();

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT_UDP);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    handleData(fd);
    close(sockfd);
    return 0;
}