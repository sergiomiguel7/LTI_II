
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
void handleBegin(char *str, char *receive);
void receiveData(char *readBuf, int index);
void showDevices();
void sendPacket();
void showMenu();
void handleOptions();
void handleStop();
int buildStartPacket(char *str, int index);
int buildStopPacket(char *str, uint8_t stopCode);

/**
 * @param sig - signal identifier
 * 
 * handles the sigint signal and closes everything 
 **/
void stopSensor(int sig)
{
    if (sig != SIGUSR1)
        return;

    pid_t pid = getpid();

    for (int i = 0; i < configuredPorts; i++)
    {
        char aux[128];
        sprintf(aux, "Recebi sinal no pid %d e estou a comparar com %d", pid, childPid);
        write(1, aux, strlen(aux));

        if (pid == childPid)
        {
            //stop to sensor
            char write[SIZE1];
            int size = buildStopPacket(write, 0);
            RS232_SendBuf(actualConfig[i].serialNumber, write, size);
            RS232_CloseComport(actualConfig[i].serialNumber);
            _exit(0);
        }
    }
}

int main()
{
    signal(SIGUSR1, stopSensor);
    configuredPorts = 0;
    showRealTime = 0;

    readConfigFile();
    handleOptions();
}

/**
 * show the current menu configured on menu.txt 
 **/
void showMenu()
{
    char buffer[4096];

    int fd = open(MENU_FILE, O_RDONLY), n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        write(1, buffer, strlen(buffer));
    }
    close(fd);
}

/**
 * handle the selected option from user
 * foreach function it's created a child process to handle the functionality
 * */
void handleOptions()
{
    char bufWrite[SIZE3], bufRead[SIZE_DATA];
    int option;
    pid_t pid;

    do
    {
        showMenu();
        printf("\nOpção: ");
        scanf("%d", &option);
        getchar();

        switch (option)
        {
        case 0:
            closeFiles();
            _exit(0);
        case 1:
            openSerial();
            handleBegin(bufWrite, bufRead);
            break;
        case 2:
            _exit(1);
            break;
        case 3:
            _exit(1);
            break;
        case 4:
            handleStop();
            break;
        default:
            printf("Opção inválida :c");
            break;
        }

    } while (option != 0);
}

void showDevices()
{
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            printf("Option: %d -> ISS: %d Pid -> %d\n", i, actualConfig[i].iss, actualConfig[i].pid);
        }
    }
}

void handleStop()
{
    showDevices();
    int device = 0;
    printf("Sensor: ");
    scanf("%d", &device);
    getchar();
    if (device >= 0 && device < configuredPorts)
    {
        childPid = actualConfig[device].pid;
        sleep(1);
        kill(actualConfig[device].pid, SIGUSR1);
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
        case 1:
            strcpy(actualConfig[configuredPorts].GPS, token);    
            break;
        case 2:
            strcpy(actualConfig[configuredPorts].area, token);    
            break;
        default:
            strcpy(actualConfig[configuredPorts].portSerial, token);
            actualConfig[configuredPorts].opened = 1;
            configuredPorts++;
            break;
        }

        nmrArgs > 2 ? nmrArgs = 0 : nmrArgs++;
        token = strtok(NULL, ";");
    }
    printf("Number of devices found: %d\n", configuredPorts);
}
