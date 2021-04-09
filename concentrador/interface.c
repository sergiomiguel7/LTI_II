
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include "api.h"
#include "rs232/rs232.h"

//define functions
void handler(int sign);
void closeFiles();
void openSerial();
void readConfigFile();
void printConfig();
void handleBegin(char *str);
void receiveData(char *readBuf);
void sendPacket();
void showMenu();
void handleOptions(int option);
int buildStartPacket(char *str, int index);
int buildStopPacket(char *str, uint8_t stopCode);

/**
 * @param sig - signal identifier
 * 
 * handles the sigint signal and closes everything 
 **/
void handler(int sig)
{
    if (sig != SIGINT)
        return;

    if (getpid() != parentPid)
    {
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
        _exit(0);
    }
}

int main()
{
    signal(SIGINT, handler);
    configuredPorts = 0;
    parentPid = getpid();

    readConfigFile();
    showMenu();
}

void showMenu()
{
    char buffer[SIZE1];

    int fd = open(MENU_FILE, O_RDONLY), n, option = 10;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        write(1, buffer, strlen(buffer));
    }
    close(fd);
    handleOptions(-1);
}

void handleOptions(int option)
{
    char bufWrite[SIZE3], bufRead[SIZE_DATA];

    while (option != 0)
    {
        printf("\nOpção: ");
        scanf("%d", &option);
        getchar();

        if (option == 0)
        {
            closeFiles();
            _exit(0);
        }
        else if (option > 3)
        {
            printf("Opção inválida :c");
            handleOptions(-1);
        }
        else
        {
            childPid = fork();
            if (!childPid)
            {
                switch (option)
                {
                case 1:
                    openSerial();
                    handleBegin(bufWrite);
                    receiveData(bufRead);
                    break;
                case 2:
                    _exit(1);
                    break;
                case 3:
                    _exit(1);
                    break;
                }
            }
            else
            {
                int child = wait(&status);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
                    write(1, "\nNot available yet :c", 21);
                else
                    showMenu();
            }
        }
    }
}

/**
 * function reads the config file declarated on api.h
 * and set all the parameters
 **/
void readConfigFile()
{
    int fd = open(CONFIG_FILE, O_RDONLY);
    char configLine[SIZE3];
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
            strcpy(actualConfig[configuredPorts].GPS, "100;200");
            strcpy(actualConfig[configuredPorts].area, "Quarto");
            configuredPorts++;
            break;
        }

        nmrArgs > 0 ? nmrArgs = 0 : nmrArgs++;
        token = strtok(NULL, ";");
    }
    printf("Number of devices found: %d\n", configuredPorts);
}
