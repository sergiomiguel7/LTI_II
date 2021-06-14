#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "rs232/rs232.h"
#include "api.h"
#include <mosquitto.h>

//socket udp
struct sockaddr_in servaddr;

void *mqtt_init(void *varg);

void receiveData(char *readBuf);

// ------- BUILD PACKETS ---------------

/**
* @param str - packet to send
* 
* @return the number of bytes to send
* 
* build the start packet to send to sensor
*/
int buildStartPacket(char *str)
{
    str[0] = (char)START;
    uint32_t ts = current_timestamp();
    split32(str + 1, ts);
    split32(str + 5, sonConfig.pa);
    return 9;
}

/**
 * @param str - packet to send
 * @param stopCode - reason to stop code
 * 
 * @return the number of bytes to send
 * 
 * build the stop packet to send to sendor
 **/
int buildStopPacket(char *str, uint8_t stopCode)
{
    str[0] = (char)STOP;
    str[1] = (char)stopCode;
    return 2;
}

/**
* @param str - packet to send
* 
* @return the number of bytes to send
* 
* build the start packet to send to sensor
*/
int buildLedPacket(char *str, uint8_t signal)
{
    str[0] = (char)LED;
    str[1] = (char)signal;
    return 2;
}

// ----------------- IO OPERATIONS -------------

void openFiles()
{
    fdData = open("log/data.csv", O_CREAT | O_RDWR | O_APPEND, 0666);
    fdErrors = open("log/errors.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
}

void openServer()
{
    char area[12];
    printf("Área associada ao concentrador: ");
    scanf("%s", area);
    char id[2];
    printf("ID concentrador: ");
    scanf("%s", id);
    pid_t pid = fork();
    udpServer = pid;

    if (pid == 0)
    {
        int err = execl("server.out", "server", area, id, NULL);
        if (err)
        {
            perror("exec err");
            _exit(1);
        }
    }
}

/*
* function to open the config file declarated portSerial to receive
* data from arduino
**/
void openSerial()
{
    for (int i = 0; i < configuredPorts; i++)
    {
        actualConfig[i].led_status = 0;
        actualConfig[i].pid = 0;
        actualConfig[i].opened = 1;

        actualConfig[i].serialNumber = RS232_GetPortnr(actualConfig[i].portSerial);

        int status = RS232_OpenComport(actualConfig[i].serialNumber, 115200, "8N1", 0);
        if (status)
        {
            actualConfig[i].opened = 0;
            //printf("Cannot open port %s :c\n", actualConfig[i].portSerial);
        }
    }
}

/**
* close all files and com ports
**/
void closeFiles()
{
    close(fdLogs);
    close(fdErrors);
    close(fdData);
}

void closePorts()
{
    kill(udpServer, SIGINT);
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            kill(actualConfig[i].pid, SIGKILL);
            RS232_CloseComport(actualConfig[i].serialNumber);
        }
    }
}

// ----------- COMMUNICATION HANDLING (RECEIVE AND SEND PACKETS) ---------------

/**
 * @param str - buffer to write on serial port to arduino
 * 
 * this function handles the starter packet and create the log files
 **/
void handleBegin(char *str, char *receive)
{
    pthread_t tid;
    if (pthread_create(&tid, NULL, mqtt_init, NULL))
    {
        perror("not created");
        exit(-1);
    }
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            pid_t pid = fork();
            actualConfig[i].pid = pid;

            if (!pid)
            {
                sonConfig = actualConfig[i];
                sonConfig.pid = getpid();
                sonConfig.realtime = 0;
                int size = buildStartPacket(str);
                RS232_SendBuf(sonConfig.serialNumber, str, size);
                receiveData(receive);
            }
        }
    }
}

/**
 * send packet to resolved issue on bluetooth connection with sensor
 * basic awake packet
 **/
void sendPacket()
{
    if (sonConfig.opened)
    {
        char str[1] = "a";
        RS232_SendBuf(sonConfig.serialNumber, str, 1);
    }
}

/**
 * @param readBuf - char buffer
 * handles incoming data from sensors
 * DATAx => |0 -> TYPE | 1 -> ISS | 2-5 -> TSP | 6 -> TGM | 7... -> VALi| , x = 1 or 2
 * ERROR => |0 -> TYPE | 1 -> ISS | 2-5 -> TSP | 6 -> ERR |
 **/
void receiveData(char *readBuf)
{
    int readed = 0, sockfd, socketOpened = 1;
    char entry[SIZE_DATA];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        socketOpened = 0;
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT_UDP);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    while (1)
    {
        if (sonConfig.opened)
        {
            readed = RS232_PollComport(sonConfig.serialNumber, readBuf, SIZE_DATA);

            if (readed > 0)
            {
                openFiles();
                if (readBuf[0] >= ERROR && readBuf[0] <= DATA2)
                {
                    sonConfig.iss = readBuf[1];
                    uint32_t timestamp = join32(readBuf + 2);

                    uint8_t type = readBuf[6];
                    //ERROR PACKET
                    if (readBuf[0] == ERROR)
                    {
                        sprintf(entry, "%u;%s;%s;%u;%u;\n",
                                sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, type);
                        write(fdErrors, entry, sizeof(entry));
                        if (socketOpened == 1)
                        {
                            sendto(sockfd, entry, strlen(entry),
                                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                                   sizeof(servaddr));
                        }
                        if (sonConfig.realtime)
                            write(1, entry, strlen(entry));
                    }
                    else
                    {
                        //DATA 1 PACKET (LDR)
                        if (readBuf[0] == DATA1)
                            for (int j = 7; j < readed; j = j + 2)
                            {
                                if (j != 7)
                                {
                                    timestamp += (sonConfig.pa / 1000);
                                }

                                int ldr = join16(readBuf + j); //ldr

                                float voltage = ((ldr * 3.3) / (4095)); //tensao
                                float val = 5 - voltage;
                                float current = (voltage / (float)3000); //corrente
                                float converted = val / (current * 1000);

                                float lux = pow(10, ((log10(converted) - 1.7782) / -5));

                                if (checkValue('v', voltage, timestamp, sockfd, socketOpened))
                                {
                                    //TODO: remove converted from sprintf
                                    sprintf(entry, "%u;%s;%s;%u;%c;%f;\n",
                                            sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, (char)type, lux);
                                    int n = write(fdData, entry, strlen(entry));
                                    if (socketOpened == 1)
                                    {
                                        sendto(sockfd, entry, strlen(entry),
                                               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                                               sizeof(servaddr));
                                    }
                                    if (sonConfig.realtime)
                                        write(1, entry, strlen(entry));
                                }
                            }
                        //DATA 2 PACKET (S)
                        /*else if (readBuf[0] == DATA2)
                        {
                            char state[12];
                            uint8_t value = readBuf[7];
                            if (checkValue('s', value, timestamp, sockfd, socketOpened))
                            {
                                sonConfig.led_status = value;
                                if (value)
                                    strcpy(state, "Ligado");
                                else
                                    strcpy(state, "Desligado");

                                sprintf(entry, "%u;%s;%s;%u;%c;%s;\n",
                                        sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, (char)type, state);
                                int n = write(fdData, entry, strlen(entry));
                                if (socketOpened == 1)
                                {
                                    sendto(sockfd, entry, strlen(entry),
                                           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                                           sizeof(servaddr));
                                }
                                if (sonConfig.realtime)
                                    write(1, entry, strlen(entry));
                            }
                        }*/
                    }
                }
                memset(entry, 0, sizeof entry);
                closeFiles();
            }
        }
        else
        {
            RS232_CloseComport(sonConfig.serialNumber);
            _exit(0);
        }
        sendPacket();
        usleep(100000); /* sleep for 100 milliSeconds */
    }
}

void writeServer(char *message, int size)
{
}

int checkValue(char type, float value, uint32_t timestamp, int socketFd, int opened)
{

    if (type == 'v')
    {
        if (value >= 0 && value <= 3.3)
            return 1;
    }

    if (type == 's')
    {
        if ((int)value >= 0 && (int)value <= 2)
            return 1;
    }

    char entry[SIZE_DATA];

    sprintf(entry, "%u;%s;%s;%u;%u;\n",
            sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, BAD_VALUE_ERR);
    write(fdErrors, entry, sizeof(entry));
    if (opened == 1)
    {
        sendto(socketFd, entry, strlen(entry),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
    }

    return 0;
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
    char entry[SIZE_DATA];
    char state[12];

    char *token = strtok(msg->payload, ";");
    uint8_t iss = atoi(token);
    token = strtok(NULL, ";");
    uint32_t timestamp = strtoul(token, NULL, 10);
    token = strtok(NULL, ";");
    if (strcmp(token, "1") == 0)
        strcpy(state, "Ligado");
    else
        strcpy(state, "Desligado");

    sonConfig.iss = iss;
    printf("%u %u %s\n",iss,timestamp,state);
    sprintf(entry, "%u;%s;%s;%u;%c;%s;\n",
            sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, 'S', state);

    //TO DO: ever
}

void *mqtt_init(void *varg)
{
    int rc, id = 1;
    mosquitto_lib_init();
    struct mosquitto *mosq;

    mosq = mosquitto_new("Concentrador", true, &id);

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
    exit(-1);
}