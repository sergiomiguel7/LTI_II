
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

int main()
{
    signal(SIGINT, handler);
    configuredPorts = 0;

    char bufWrite[1024];
    char bufRead[1024];

  //  int coms = comEnumerate();
    readConfigFile();
    openSerial();
    for(int i = 0; i < 30 ; i++)
        handleBegin(bufWrite);
}

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
    //comTerminate();
}

/**
* close all files and com ports
**/
void closeFiles()
{
    close(fdLogs);
    close(fdErrors);
    close(fdData);
   //TODO: close with for to all actualconfig[i].serialNumber;
}

/**
 * @param str - buffer to write on serial port to arduino
 * 
 * this function handles the starter packet
 **/
void handleBegin(char *str)
{
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            printf("%s \n",str);
            int size = buildStartPacket(str, i);
            RS232_SendBuf(actualConfig[i].serialNumber, str, size);
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
        actualConfig[i].serialNumber = RS232_GetPortnr(actualConfig[i].portSerial);
        if (actualConfig[i].serialNumber == -1)
        {
            actualConfig[i].serialNumber = 6;
        }
        int status =  RS232_OpenComport(actualConfig[i].serialNumber, 115200, "8N1", 0);
        if (status)
            actualConfig[i].opened = status;
    }
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

    printConfig();
}

/**
* function to print the readed configuration
**/
void printConfig()
{
    printf("Serial Port: %s\n", actualConfig[0].portSerial);
    printf("Period between reading from sensor: %d\n", actualConfig[0].pa);
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
