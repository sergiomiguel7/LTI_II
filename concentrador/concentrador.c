
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "api.h"
#include "rs232/rs232.h"

//define functions
void openSerial();
void readConfigFile();
void printConfig();
int buildStartPacket(char *str);
int buildStopPacket(char *str, uint8_t stopCode);

int main()
{
    char bufWrite[1024];
    char bufRead[1024];

    buildStartPacket(bufWrite);

    readConfigFile();

    int coms = comEnumerate();
    openSerial();
}


/*
* function to open the config file declarated portSerial to receive
* data from arduino
**/
void openSerial()
{
    actualConfig.serialNumber = comFindPort(actualConfig.portSerial);
    if (actualConfig.serialNumber == -1)
    {
        actualConfig.serialNumber = 6;
    }
    comOpen(actualConfig.serialNumber, 115200);
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
            actualConfig.pm = atoi(token);
            break;
        case 1: 
            actualConfig.pa = atoi(token);
            break;
        case 2: 
            actualConfig.na = atoi(token);
            break;    
        default:
            break;
        }
        //read and store on actualConfig

        nmrArgs++;
        token = strtok(NULL, ";");
    }

    printConfig();
}


/**
* function to print the readed configuration
**/
void printConfig()
{
   // printf("Serial Port: %s", actualConfig.portSerial);
    printf("Period between incoming data from sensor: %d\n", actualConfig.pm);
    printf("Period between reading from sensor: %d\n", actualConfig.pa);
    printf("Number of samples before sending stop packet: %d\n", actualConfig.na);
}


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
	split32(str + 5, actualConfig.pm);
	split32(str + 9, actualConfig.pa);
	str[13] = actualConfig.na;
	return 14;
}

/**
 * @param str - packet to send
 * @param stopCode - reason to stop code
 * 
 * @return the number of bytes to send
 * 
 * build the stop packet to send to sendor
 **/
int buildStopPacket(char *str, uint8_t stopCode){
    str[0]=(char)STOP;
    str[1]=(char)stopCode;
    return 2;
}