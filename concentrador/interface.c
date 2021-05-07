
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
void closePorts();
void closeFiles();
void openSerial();
void openServer();
void readConfigFile();
void printConfig();
void handleBegin(char *str, char *receive);
void sendPacket();
void handleOptions();
void handleStop();
void showData();
void handlePositionChange();
int enableRealTime();
int showMenu();
int showDevices();
int handleChangeStatus(char *buffer);
int buildStartPacket(char *str, int index);
int buildStopPacket(char *str, uint8_t stopCode);
int buildLedPacket(char *str, uint8_t signal);

int stopAux = 0;

// <-------------SECTION SIGNALS -------------->

/**
 * @param sig - signal identifier
 * 
 * handles the sigint signal and close one sensor
 **/
void stopSensor(int sig)
{
    if (sig != SIGUSR1 && getpid() == serverPid)
        return;

    //stop to sensor
    char write[SIZE1];
    int size = buildStopPacket(write, 0);
    RS232_SendBuf(sonConfig.serialNumber, write, size);
    sleep(2);
    sonConfig.opened = 0;
}

/**
 * 
 * @param sig - signal identifier
 * 
 * handles the real time data
 * */
void changeRealTime(int sig)
{
    if (sig != SIGUSR2 && getpid() == serverPid)
        return;

    if (sonConfig.realtime)
        sonConfig.realtime = 0;
    else
        sonConfig.realtime = 1;

}

/**
 * 
 * @param sig - signal identifier
 * 
 * stop the real time data
 * */
void stopRealTime(int sig)
{
    if (sig != SIGINT && getpid() != serverPid)
        return;
}

int main()
{
    signal(SIGUSR1, stopSensor);
    signal(SIGUSR2, changeRealTime);
    signal(SIGINT, stopRealTime);

    concentrador_id = 1;
    configuredPorts = 0;
    serverPid = getpid();

    readConfigFile();
    openFiles();
    handleOptions();
}

// <-------------SECTION MENU -------------->

/**
 * show the current menu configured on menu.txt 
 **/
int showMenu()
{
    int option = 10;
    printf("\nBem vindo ao menu de admininstração!\n");
    printf("1 - Arrancar sistema\n2 - Definir estado led\n3 - Atualizar posição sensor\n4 - Visualização de dados\n5 - Parar um sensor\n0 - Sair");
    printf("\nOpção: ");
    scanf("%d", &option);
    getchar();

    return option;
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
        option = showMenu();

        switch (option)
        {
        case 0:
            closeFiles();
            closePorts();
            sleep(2);
            _exit(0);
        case 1:
            openSerial();
            handleBegin(bufWrite, bufRead);
            openServer();
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
            {
                enableRealTime();
            }
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

    } while (option != 0);
}

// <-------------SECTION DATA VISUALIZATION -------------->

/**
 * send a signal to all child to open real data income
 * 
 * */
int enableRealTime()
{
    int counter = 0;
    while (counter < 2)
    {
        if (stopAux == 0)
            stopAux = 1;
        else
            stopAux = 0;

        for (int i = 0; i < configuredPorts; i++)
        {
            if (actualConfig[i].opened)
            {
                kill(actualConfig[i].pid, SIGUSR2);
            }
        }

        if (stopAux) //wait receiving sigint signal to close real time
        {
            pause();
        }
        counter++;
    }
}

/**
 * show data between two dates 
 * 
 **/
void showData()
{

    char configLine[SIZE3], date1[SIZE1], hour1[SIZE1], date2[SIZE1], hour2[SIZE1], aux[SIZE2];
    int sizeRead, counter = 0;
    printf("\nInsira a primeira data formato (DD/MM/AAAA): ");
    scanf("%s", date1);
    printf("Insira a primeira hora (HH:MM): ");
    scanf("%s", hour1);
    printf("\nInsira a segunda data formato (DD/MM/AAAA): ");
    scanf("%s", date2);
    printf("Insira a segunda hora formato (HH:MM): ");
    scanf("%s", hour2);

    long time1 = transform_data(date1, hour1);
    long time2 = transform_data(date2, hour2);

    while ((sizeRead = read(fdData, configLine, sizeof(configLine))) > 0)
    {
        char *endstr; //buffer to lines readed from file
        char *token = strtok_r(configLine, "\n", &endstr);

        while (token != NULL)
        {
            counter = 0;
            strcpy(aux, token);
            char *line; //buffer to token line
            char *token2 = strtok_r(token, ";", &line);
            while (token2 != NULL)
            {
                if (counter == 3)
                {
                    long timeLine = (long)atoi(token2);
                    if (timeLine > time1 && timeLine < time2)
                    {
                        char linePrint[SIZE2];
                        sprintf(linePrint, "%s\n", aux);
                        write(1, linePrint, strlen(linePrint));
                    }
                    break;
                }
                counter++;
                token2 = strtok_r(NULL, ";", &line);
            }
            memset(aux, 0, sizeof aux);
            token = strtok_r(NULL, "\n", &endstr);
        }
        memset(configLine, 0, sizeof configLine);
    }
}

/**
 * print on the screen all the opened sensors
 * */
int showDevices()
{
    int total = 0;
    printf("\nSensores disponíveis: \n");
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened == 1)
        {
            total++;
            printf("Option: %d - ISS: %d Área: %s Estado: %d", i, actualConfig[i].iss, actualConfig[i].area ,actualConfig[i].led_status);
        }
    }
    return total;
}

// <-------------SECTION STATUS CHANGE -------------->

/**
 * function to handle the stop of one sensor
 * */
void handleStop()
{
    int total = showDevices();
    if (!total)
        return;
    int device = 0, status;
    printf("\n\nSensor: ");
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
    int total = showDevices(), option = 4;
    if (!total)
        return 0;
    int device = 0;
    printf("\n\nSensor: ");
    scanf("%d", &device);
    getchar();
    printf("\n0-Desligar\n1-Ligado\n2-Default(sensor movimento)\nOpção: ");
    scanf("%d", &option);
    if (device >= 0 && device < configuredPorts)
    {
        if (option >= 0 && option < 4)
        {
            actualConfig[device].led_status = option;

            int size = buildLedPacket(buffer, option);
            RS232_SendBuf(actualConfig[device].serialNumber, buffer, size);
            return 1;
        }
        else
            return 0;
    }
    return 0;
}

// <-------------SECTION IO -------------->

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
    printf("\n\nSensor: ");
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
