
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "api.h"
#include "rs232/rs232.h"

//define functions
void openSerial();
void readConfigFile();

int main()
{
    char bufWrite[1024];
    char bufRead[1024];

    buildStartPacket(bufWrite, actualConfig.pm, actualConfig.pa, actualConfig.ns);

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
* function to print the readed configuration
**/
void printConfig()
{
    printf("Serial Port: %s", actualConfig.portSerial);
}

/**
 * function reads the config file declarated on api.h
 * and set all the parameters
 **/
void readConfigFile()
{

    FILE *fd;
    char *configLine = NULL;
    int sizeRead, nmrArgs = 0;

    fd = open(CONFIG_FILE, "r");
    if (fd == NULL)
    {
        return -1;
    }

    sizeRead = getline(configLine, 0, fd);
    char *token = strtok(configLine, ";");

    while (token != NULL)
    {

        //read and store on actualConfig

        nmrArgs++;
        token = strtok(NULL, ";");
    }

    printConfig();
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