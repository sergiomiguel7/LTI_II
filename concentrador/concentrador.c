
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
int buildStartPacket(char *str, uint32_t pm, uint32_t pa, uint32_t ns);

int main()
{
    char bufWrite[1024];
    char bufRead[1024];

    buildStartPacket(bufWrite, actualConfig.pm, actualConfig.pa, actualConfig.na);

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
    printf("Serial Port: %s", actualConfig.portSerial);
}


/*
* build the start packet to send to sensor
*/
int buildStartPacket(char *str, uint32_t pm, uint32_t pa, uint32_t ns)
{
	str[0] = (char)0;
	uint32_t ts = current_timestamp();
	split32(str + 1, ts);
	split32(str + 5, pm);
	split32(str + 9, pa);
	str[13] = ns;
	return 14;
}