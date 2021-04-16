#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include "rs232/rs232.h"
#include "api.h"

void receiveData(char *readBuf, int index);

// ------- BUILD PACKETS ---------------

/**
* @param str - packet to send
* 
* @return the number of bytes to send
* 
* build the start packet to send to sensor
*/
int buildStartPacket(char *str, int index)
{
    str[0] = (char)START;
    uint32_t ts = current_timestamp();
    split32(str + 1, ts);
    split32(str + 5, actualConfig[index].pa);
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

/*
* function to open the config file declarated portSerial to receive
* data from arduino
**/
void openSerial()
{
    for (int i = 0; i < configuredPorts; i++)
    {
        actualConfig[i].led_status = 0;
        actualConfig[i].opened = 1;
        actualConfig[i].serialNumber = RS232_GetPortnr(actualConfig[i].portSerial);

        int status = RS232_OpenComport(actualConfig[i].serialNumber, 115200, "8N1", 0);
        if (status)
        {
            actualConfig[i].opened = 0;
            printf("Cannot open port %s :c\n", actualConfig[i].portSerial);
        }
    }

    fdData = open("log/data.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
    fdErrors = open("log/errors.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
}


/**
* close all files and com ports
**/
void closeFiles()
{
    close(fdLogs);
    close(fdErrors);
    close(fdData);
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
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
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            pid_t pid = fork();
            actualConfig[i].pid = pid;

            if (!pid)
            {
                actualConfig[i].pid = getpid();
                int size = buildStartPacket(str, i);
                RS232_SendBuf(actualConfig[i].serialNumber, str, size);
                receiveData(receive, i);
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
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            char str[1] = "a";
            RS232_SendBuf(actualConfig[i].serialNumber, str, 1);
        }
    }
}

/**
 * @param readBuf - char buffer
 * handles incoming data from sensors
 * DATAx => |0 -> TYPE | 1 -> ISS | 2-5 -> TSP | 6 -> TGM | 7... -> VALi| , x = 1 or 2
 * ERROR => |0 -> TYPE | 1 -> ISS | 2-5 -> TSP | 6 -> ERR |
 **/
void receiveData(char *readBuf, int index)
{
    int readed = 0;
    char entry[SIZE_DATA];
    while (1)
    {
        if (actualConfig[index].opened)
        {
            readed = RS232_PollComport(actualConfig[index].serialNumber, readBuf, SIZE_DATA);

            if (readed > 0)
            {
                if (readBuf[0] >= ERROR && readBuf[0] <= DATA2)
                {
                    actualConfig[index].iss = readBuf[1];
                    uint32_t timestamp = join32(readBuf + 2);

                    uint8_t type = readBuf[6];
                    //ERROR PACKET
                    if (readBuf[0] == ERROR)
                    {
                        sprintf(entry, "%u;%s;%s;%u;%u;\n",
                                actualConfig[index].iss, actualConfig[index].area, actualConfig[index].GPS, timestamp, type);
                        write(fdErrors, entry, sizeof(entry));
                        printf("ERROR =>  ISS: %u, TIMESTAMP: %u, ERRO: %u\n", actualConfig[index].iss, timestamp, type);
                    }
                    else
                    {
                        //DATA 1 PACKET (V)
                        if (readBuf[0] == DATA1)
                            for (int j = 7; j < readed; j = j + 4)
                            {
                                if(j!=7){
                                    timestamp += timestamp + actualConfig[index].pa;
                                }
                                float value = joinFloat(readBuf + j);
                                sprintf(entry, "%u;%s;%s;%u;%c;%f\n",
                                        actualConfig[index].iss, actualConfig[index].area, actualConfig[index].GPS, timestamp, (char)type, value);
                                int n = write(fdData, entry, strlen(entry));
                                //printf("Recebi valor %f e escrevi no ficheiro %d\n", value, n);
                            }
                        //DATA 2 PACKET (S)
                        else if (readBuf[0] == DATA2)
                        {
                            char state[12];
                            uint8_t value = readBuf[7];
                            actualConfig[index].led_status = value;
                            if (value)
                                strcpy(state, "Ligado");
                            else
                                strcpy(state, "Desligado");

                            sprintf(entry, "%u;%s;%s;%u;%c;%s\n",
                                    actualConfig[index].iss, actualConfig[index].area, actualConfig[index].GPS, timestamp, (char)type, state);
                            int n = write(fdData, entry, strlen(entry));
                            //printf("Recebi valor %d e escrevi no ficheiro %d\n", value, n);
                        }
                    }
                }
                memset(entry, 0, sizeof entry);
            }
        }
        sendPacket();
        usleep(100000); /* sleep for 100 milliSeconds */
    }
}