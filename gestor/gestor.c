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
#include <mosquitto.h>
#define SA struct sockaddr
#define PORT_TCP 7778
#define MAXLINE 1024
#define MAX 128

void connectDB();
void insertDB();
void *func(void *arg);
void *mqtt_init(void *varg);
MYSQL *con;

int main()
{
    int sockfd, connfd, client_sock, len;
    struct sockaddr_in servaddr, cli;
    pthread_t tid;

    pthread_t ted;

    connectDB();

    if (pthread_create(&ted, NULL, mqtt_init, NULL))
    {
        perror("not created");
        exit(-1);
    }

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
            }
            else
            {
                insertDB(buff);
            }

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
    else
    {
        printf("Connected to database\n");
    }
}

void insertDB(char *buff)
{
    MYSQL_RES *res;
    MYSQL_ROW row;

    int user_exist = 1;
    int id_user;

    int counter = 0;
    char buffer[1024];

    char user[40];
    char area[24];
    char area_sensor[25];
    int id_concentrador;

    char unidade[1];
    float valor;
    int id_sensor;
    uint32_t timestamp;

    write(1, buff, strlen(buff));

    char *token = strtok(buff, ";");

    while (token != NULL)
    {
        switch (counter)
        {
        case 0:
            strcpy(user, token);
            break;
        case 1:
            strcpy(area, token);
            break;
        case 2:
            id_concentrador = atoi(token);
            break;
        case 3:
            id_sensor = atoi(token);
            break;
        case 4:
            strcpy(area_sensor, token);
            break;
        case 6:
            timestamp = strtoul(token, NULL, 10);
            break;
        case 7:
            strcpy(unidade, token);
            break;
        case 8:
            valor = atof(token);
            break;
        default:
            break;
        }
        counter++;
        token = strtok(NULL, ";");
    }

    bzero(buffer, 0);

    //GET ID USERNAME
    sprintf(buffer, "SELECT id FROM user WHERE username = '%s'", user);
    if (mysql_query(con, buffer) != 0)
    {
        fprintf(stderr, "Query Failure for getting user\n");
    }
    res = mysql_store_result(con);

    if (mysql_num_rows(res) == 0) //user nao existe
    {
        user_exist = 0;
    }
    else
    {
        user_exist = 1;
        row = mysql_fetch_row(res);
        id_user = atoi(row[0]);
    }

    mysql_free_result(res);

    if (user_exist == 1)
    {
        bzero(buffer, 0);
        //INSERIR DADO
        sprintf(buffer, "INSERT INTO dado(unidade,valor,id_sensor,id_concentrador,timestamp) VALUES ('%s', '%f', %d, %d, %u)",
                unidade, valor, id_sensor, id_concentrador, timestamp);

        if (mysql_query(con, buffer) != 0)
        {
            fprintf(stderr, "Query Failure 4\n");
        }
        else
        {
            write(1, "Inserido com sucesso\n", 10);
        }
    }
}

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    if (rc)
    {
        printf("Error with result code: %d\n", rc);
        exit(-1);
    }
    mosquitto_subscribe(mosq, NULL, "room/mov", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    float state;

    char *token = strtok(msg->payload, ";");
    int iss = atoi(token);
    token = strtok(NULL, ";");
    uint32_t timestamp = strtoul(token, NULL, 10);
    token = strtok(NULL, ";");
    state = atoi(token);

    char buffer[1024];

    sprintf(buffer, "SELECT id_concentrador FROM sensor WHERE id = %d", iss);

    mysql_query(con, buffer);

    res = mysql_store_result(con);

    row = mysql_fetch_row(res);
    int id_concentrador = atoi(row[0]);

    mysql_free_result(res);

    bzero(buffer, 0);
    //INSERIR DADO
    sprintf(buffer, "INSERT INTO dado(unidade,valor,id_sensor,id_concentrador,timestamp) VALUES ('%s', %f, %d, %d, %u)",
            "S", state, iss, id_concentrador, timestamp);

    if (mysql_query(con, buffer) != 0)
    {
        fprintf(stderr, "Query Failure 4\n");
    } else{
        write(1, "Inserido com sucesso\n", 10);
    }
}

void *mqtt_init(void *varg)
{
    int rc, id = 1;
    mosquitto_lib_init();
    struct mosquitto *mosq;

    mosq = mosquitto_new("Gestor", true, &id);

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, "localhost", 1883, 10);

    if (rc)
    {
        printf("Could not connect to Broker with return code %d\n", rc);
        exit(-1);
    }

    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return NULL;
}