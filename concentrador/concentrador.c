
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "api.h"
#include "rs232.h"

//define functions
void openSerial();
void readConfigFile();

int main()
{

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