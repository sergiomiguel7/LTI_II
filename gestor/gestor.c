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
#include <mysql/mysql.h>
#include "../concentrador/server_api.h"
#define SA struct sockaddr
#define PORT_TCP 7778
#define MAXLINE 1024
#define MAX 128


void connectDB();
void insertDB();
void *func(void *arg);
MYSQL *con;

int main()
{
    int sockfd, connfd, client_sock, len;
    struct sockaddr_in servaddr, cli;
    pthread_t tid;

    connectDB();
    

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
            insertDB(buff);
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

    //MYSQL OPERATIONS
    void connectDB()
    {
        con = mysql_init(NULL);
        if (con == NULL)
        {
            printf("%s\n", mysql_error(con));
            exit(1);
        }

        if (mysql_real_connect(con, "localhost", "root", "Vitoriag7!",
                               "sensoresdb", 0, NULL, 0) == NULL)
        {
            printf("%s\n", mysql_error(con));
            mysql_close(con);
            exit(1);
        } 
        else{
            printf("Connected to database\n");

        }
    }

    void insertDB(char buff)
    {
    MYSQL *conn;
    MYSQL_RES res;
    MYSQL_ROW row;

    conn = mysql_init(NULL);
    char buffer [212];
    int id;
    char unidade;
    int valor;
    int id_sensor;
    int id_concentrador;
    uint32_t timestamp;

    while( p!NULL ){
        id= buff[0];
        unidade=buff[1];
        valor=buff[2];
        id_sensor=buff[3];
        id_concentrador=buff[4];
        timestamp=buff[5];
        memset(buffer,0,212);
        sprintf(buffer, "INSERT INTO dado(id,unidade,valor,id_sensor,id_concentrador,timestamp) VALUES (%d, '%s', '%d', %d, %d, '%d')",
             id, unidade, valor, id_sensor, id_concentrador, timestamp);
    }

    
    if (mysql_real_connect(conn, "localhost", "root", "Vitoriag7!", "sensoresdb", 0, NULL, 0) == NULL)
        {
            printf("%s\n", mysql_error(conn));
            mysql_close(conn);
            exit(1);
        }

    if (mysql_query(conn, comando) != 0){                                                                                                  
        fprintf(stderr, "Query Failure\n");                                                              
        exit(0);                                                                            
    } 
    mysql_close(conn);    
    }

