
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include "api.h"
#include "rs232/rs232.h"

//define functions
void handler(int sign);
void closeFiles();
void openSerial();
void readConfigFile();
void printConfig();
void handleBegin(char *str);
int buildStartPacket(char *str, int index);
int buildStopPacket(char *str, uint8_t stopCode);
void receiveData(char *readBuf);

/**
 * @param sig - signal identifier
 * 
 * handles the sigint signal and closes everything 
 **/
void handler(int sig)
{
    if (sig != SIGINT)
        return;

    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            //stop to sensor
            char write[SIZE1];
            int size = buildStopPacket(write, 0);
            RS232_SendBuf(actualConfig[i].serialNumber, write, size);
        }
    }
    //close all
    closeFiles();
}

int main()
{
    signal(SIGINT, handler);
    configuredPorts = 0;

    char bufWrite[SIZE3];
    char bufRead[SIZE_DATA];

    readConfigFile();
    openSerial();
    handleBegin(bufWrite);
    receiveData(bufRead);
}

/**
 * function reads the config file declarated on api.h
 * and set all the parameters
 **/
void readConfigFile()
{
    int fd = open(CONFIG_FILE, O_RDONLY);
    char configLine[1024];
    int sizeRead, nmrArgs = 0;

    if (fd < 0)
    {
        perror("erro reading file");
        return;
    }

    sizeRead = read(fd, configLine, sizeof(configLine));

    char *token = strtok(configLine, ";");

    while (token != NULL)
    {
        switch (nmrArgs)
        {
        case 0:
            actualConfig[configuredPorts].pa = atoi(token);
            break;

        default:
            strcpy(actualConfig[configuredPorts].portSerial, token);
            actualConfig[configuredPorts].opened = 1;
            configuredPorts++;
            break;
        }

        nmrArgs > 0 ? nmrArgs = 0 : nmrArgs++;
        token = strtok(NULL, ";");
    }
    printf("Number of devices found: %d\n", configuredPorts);
}

/*
* function to open the config file declarated portSerial to receive
* data from arduino
**/
void openSerial()
{
    for (int i = 0; i < configuredPorts; i++)
    {
        actualConfig[i].serialNumber = RS232_GetPortnr(actualConfig[i].portSerial);
        if (actualConfig[i].serialNumber == -1)
        {
            actualConfig[i].serialNumber = 6;
        }
        int status = RS232_OpenComport(actualConfig[i].serialNumber, 115200, "8N1", 0);
        if (status)
        {
            actualConfig[i].opened = status;
            printf("Cannot open port %s :c\n", actualConfig[i].portSerial);
        }
    }
}

/**
 * @param str - buffer to write on serial port to arduino
 * 
 * this function handles the starter packet and create the log files
 **/
void handleBegin(char *str)
{
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            printf("%s \n", str);
            int size = buildStartPacket(str, i);
            RS232_SendBuf(actualConfig[i].serialNumber, str, size);
        }
    }

    fdData = open("log/data.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
    fdErrors = open("log/errors.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
}

/**
 * @param readBuf - char buffer
 * handles incoming data from sensors
 * DATAx => |0 -> TYPE | 1 -> ISS | 2-5 -> TSP | 6 -> TGM | 7... -> VALi| , x = 1 or 2
 * ERROR => |0 -> TYPE | 1 -> ISS | 2-5 -> TSP | 6 -> ERR |
 **/
void receiveData(char *readBuf)
{
    int readed = 0;
    while (1)
    {
        for (int i = 0; i < configuredPorts; i++)
        {
            readed = RS232_PollComport(actualConfig[i].serialNumber, readBuf, SIZE_DATA - 1);
            printf("Li da COM %d => do config number: %d de porta serial: %s\n", readed, actualConfig[i].serialNumber, actualConfig[i].portSerial);

            if (readed > 0)
            {
                if (readBuf[0] >= ERROR && readBuf[0] <= DATA2)
                {
                    actualConfig[i].iss = readBuf[1];
                    uint32_t timestamp = join32(readBuf + 2);

                    uint8_t type = readBuf[6];

                    if (readBuf[0] == ERROR)
                    {
                        /*                         
                        char entry[SIZE1];
                        sprintf(entry, "%u;%s;%s;%u;%u;\n",
                        actualConfig[i].iss, actualConfig[i].area, actualConfig[i].GPS, timestamp, type);
                        write(fdErrors, entry, sizeof(entry)); */
                        printf("ERROR =>  ISS: %u, TIMESTAMP: %u, ERRO: %u\n", actualConfig[i].iss, timestamp, type);
                    }
                    else
                    {
                        for (int j = 7; j < readed; j = j + 4)
                        {
                            float value = joinFloat(readBuf + j);

                            printf("DATA =>  ISS: %u, TIMESTAMP: %u, TIPO: %c, VALOR: %f\n", actualConfig[i].iss, timestamp, (char)type, value);
                            /*  
                            char entry[SIZE1];
                            sprintf(entry, "%u;%s;%s;%u;%c;%f\n",
                            actualConfig[i].iss, actualConfig[i].area, actualConfig[i].GPS, timestamp, (char)type, value);
                            int n = write(fdData, entry, sizeof(entry));
                            printf("escrevi: %d com o seguinte payload: %s\n", n, entry);*/
                        }
                    }
                }
            }
            usleep(100000); /* sleep for 100 milliSeconds */
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
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            RS232_CloseComport(actualConfig[i].serialNumber);
        }
    }
}

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
