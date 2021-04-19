
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
void openFiles();
void closeFiles();
void openSerial();
void readConfigFile();
void printConfig();
void handleBegin(char *str, char *receive);
void receiveData(char *readBuf, int index);
void sendPacket();
void showMenu();
void handleOptions();
void handleStop();
void enableRealTime();
void showData();
void handlePositionChange();
int showDevices();
int handleChangeStatus(char *buffer);
int buildStartPacket(char *str, int index);
int buildStopPacket(char *str, uint8_t stopCode);
int buildLedPacket(char *str, uint8_t signal);

/**
 * @param sig - signal identifier
 * 
 * handles the sigint signal and close one sensor
 **/
void stopSensor(int sig)
{
    if (sig != SIGUSR1)
        return;

    pid_t pid = getpid();

    for (int i = 0; i < configuredPorts; i++)
    {
        if (pid == actualConfig[i].pid)
        {
            //stop to sensor
            char write[SIZE1];
            int size = buildStopPacket(write, 0);
            RS232_SendBuf(actualConfig[i].serialNumber, write, size);
            sleep(2);
            RS232_CloseComport(actualConfig[i].serialNumber);
            _exit(0);
        }
    }
}

/**
 * 
 * @param sig - signal identifier
 * 
 * handles the real time data
 * */
void changeRealTime(int sig)
{
    if (sig != SIGUSR2)
        return;

    showRealTime = 1;

    pid_t pid = getpid();

    for (int i = 0; i < configuredPorts; i++)
    {
        if (pid == actualConfig[i].pid)
        {
            showRealTime = !showRealTime;
        }
    }
}

/**
 * 
 * @param sig - signal identifier
 * 
 * stop the real time data
 * */
void stopRealTime(int sig)
{
    if (sig != SIGINT)
        return;

    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            kill(actualConfig[i].pid, SIGUSR2);
        }
    }
}

int main()
{
    signal(SIGINT, stopRealTime);
    signal(SIGUSR1, stopSensor);
    signal(SIGUSR2, changeRealTime);
    configuredPorts = 0;
    showRealTime = 0;

    readConfigFile();
    openFiles();
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
    int option, option2;
    pid_t pid;

    do
    {
        if (!showRealTime)
        {
            showMenu();
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
                if (handleChangeStatus(bufWrite))
                    printf("Estado alterado com sucesso! :) \n");
                else
                    printf("Erro ao alterar estado :C\n");
                break;
            case 3:
                handlePositionChange();
                break;
            case 4:
                //visualização de dados
                option2 = 3;
                printf("\n1-Tempo Real\n2-Dados armazenados\nOpção: ");
                scanf("%d", &option2);
                if (option2 == 1)
                    enableRealTime();
                else if (option2 == 2)
                    showData();
                break;
            case 5:
                handleStop();
                break;
            default:
                printf("Opção inválida :c");
                break;
            }
        }

    } while (option != 0);
}

/**
 * send a signal to all child to open real data income
 * 
 * */
void enableRealTime()
{
    showRealTime = 0;
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            kill(actualConfig[i].pid, SIGUSR2);
        }
    }
}

/**
 * show data between two dates 
 * 
 **/
void showData()
{

    char configLine[SIZE3], date1[SIZE1], hour1[SIZE1], date2[SIZE1], hour2[SIZE1];
    int sizeRead, counter = 0;
    printf("\nInsira a primeira data formato (DD/MM/AAAA): ");
    scanf("%s", date1);
    printf("\nInsira a primeira hora (HH:MM): ");
    scanf("%s", hour1);
    printf("\nInsira a segunda data formato (DD/MM/AAAA): ");
    scanf("%s", date2);
    printf("\nInsira a segunda hora formato (HH:MM): ");
    scanf("%s", hour2);

    long time1 = transform_data(date1, hour1);
    long time2 = transform_data(date2, hour2);

    sizeRead = read(fdData, configLine, sizeof(configLine));
    char *token = strtok(configLine, "\n");

    while (token != NULL)
    {
        counter = 0;
        char *line = strtok(token, ";");
        while (line != NULL)
        {
            if (counter == 3)
            { //compare timestamp
                long timeLine = (long)atoi(line);
                printf("line: %ld , date1: %ld , date2: %ld", timeLine, time1, time2);
                if (timeLine > time1 && timeLine < time2)
                {
                    char aux[SIZE_DATA];
                    sprintf(aux, "%s\n", token);
                    write(1, aux, strlen(aux));
                }
                break;
            }
            counter++;
            token = strtok(NULL, ";");
        }
        token = strtok(NULL, "\n");
    }
}

/**
 * print on the screen all the opened sensors
 * */
int showDevices()
{
    int total = 0;
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            total++;
            printf("\n%d -> ISS: %d Led status: %d", i, actualConfig[i].iss, actualConfig[i].led_status);
        }
    }
    return total;
}

/**
 * function to handle the stop of one sensor
 * */
void handleStop()
{
    int total = showDevices();
    if (!total)
        return;
    int device = 0, status;
    printf("\nSensor: ");
    scanf("%d", &device);
    getchar();
    if (device >= 0 && device < configuredPorts)
    {
        kill(actualConfig[device].pid, SIGUSR1);
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            actualConfig[device].opened = 0;
            actualConfig[device].pid = 0;
            printf("Sensor parado com sucesso\n");
        }
    }
}

/**
 * 
 * function to handle the change of led status on one sensor
 * 
 * */
int handleChangeStatus(char *buffer)
{
    int total = showDevices();
    if (!total)
        return 0;
    int device = 0;
    printf("\nSensor: ");
    scanf("%d", &device);
    getchar();
    if (device >= 0 && device < configuredPorts)
    {
        int signal = 0;
        if (!actualConfig[device].led_status)
            signal = 1;
        else
            signal = 0;

        actualConfig[device].led_status = signal;

        int size = buildLedPacket(buffer, signal);
        RS232_SendBuf(actualConfig[device].serialNumber, buffer, size);
        return 1;
    }
    return 0;
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

void handlePositionChange()
{
    char newGPS[SIZE0];
    char newArea[SIZE0];
    int total = showDevices();
    if (!total)
        return;
    int device = 0, status;
    printf("\nSensor: ");
    scanf("%d", &device);
    getchar();
    printf("Novas coordenadas GPS: ");
    scanf("%s", newGPS);
    printf("Nova Area: ");
    scanf("%s", newArea);

    FILE *fPrin;
    FILE *fTemp;

    //int fd = open(CONFIG_FILE, O_RDWR);
    fPrin = fopen(CONFIG_FILE, "r");
    fTemp = fopen("replace.tmp", "w");
    char configLine[SIZE3];
    char newLine[SIZE3];
    if (device >= 0 && device < configuredPorts)
    {
        int count = 0;
        while ((fgets(configLine, SIZE3, fPrin)) != NULL)
        {
            printf("Entrei\n");
            if (count == device)
            {
                sprintf(newLine, "%d;%s;%s;%s;",
                        actualConfig[device].pa, newGPS, newArea, actualConfig[device].portSerial);

                fputs(newLine, fTemp);
            }
            else
                fputs(configLine, fTemp);

            count++;
        }
    }

    fclose(fPrin);
    fclose(fTemp);

    remove(CONFIG_FILE);

    rename("replace.tmp", CONFIG_FILE);
}
