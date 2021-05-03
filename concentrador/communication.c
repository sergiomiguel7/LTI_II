#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include "rs232/rs232.h"
#include "api.h"

void receiveData(char *readBuf);

// ------- BUILD PACKETS ---------------

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
    split32(str + 5, sonConfig.pa);
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

/**
* @param str - packet to send
* 
* @return the number of bytes to send
* 
* build the start packet to send to sensor
*/
int buildLedPacket(char *str, uint8_t signal)
{
    str[0] = (char)LED;
    str[1] = (char)signal;
    return 2;
}

// ----------------- IO OPERATIONS -------------

void openFiles()
{
    fdData = open("log/data.csv", O_RDWR | O_CREAT | O_APPEND, 0666);
    fdErrors = open("log/errors.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
}

void openServer(){
    pid_t pid = fork();
    if(pid == 0){
        int err = execl("out/server.o", "server",NULL);
        if(err){
            _exit(1);
        }
    }
    sleep(3);
    int fd = open(FIFO, O_WRONLY, 0666);
    char aux[2];
    sprintf(aux, "%d", concentrador_id);
    write(fd, aux, strlen(aux));
    close(fd);
}

/*
* function to open the config file declarated portSerial to receive
* data from arduino
**/
void openSerial()
{
    for (int i = 0; i < configuredPorts; i++)
    {

        actualConfig[i].led_status = 0;
        actualConfig[i].pid = 0;
        actualConfig[i].opened = 1;

        actualConfig[i].serialNumber = RS232_GetPortnr(actualConfig[i].portSerial);

        int status = RS232_OpenComport(actualConfig[i].serialNumber, 115200, "8N1", 0);
        if (status)
        {
            actualConfig[i].opened = 0;
            //printf("Cannot open port %s :c\n", actualConfig[i].portSerial);
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
}

void closePorts()
{
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            kill(actualConfig[i].pid, SIGKILL);
            RS232_CloseComport(actualConfig[i].serialNumber);
        }
    }
}

// ----------- COMMUNICATION HANDLING (RECEIVE AND SEND PACKETS) ---------------

/**
 * @param str - buffer to write on serial port to arduino
 * 
 * this function handles the starter packet and create the log files
 **/
void handleBegin(char *str, char *receive)
{
    for (int i = 0; i < configuredPorts; i++)
    {
        if (actualConfig[i].opened)
        {
            pid_t pid = fork();
            actualConfig[i].pid = pid;

            if (!pid)
            {
                sonConfig = actualConfig[i];
                sonConfig.pid = getpid();
                sonConfig.realtime = 0;
                int size = buildStartPacket(str);
                RS232_SendBuf(sonConfig.serialNumber, str, size);
                receiveData(receive);
            }
        }
    }
}

/**
 * send packet to resolved issue on bluetooth connection with sensor
 * basic awake packet
 **/
void sendPacket()
{
    if (sonConfig.opened)
    {
        char str[1] = "a";
        RS232_SendBuf(sonConfig.serialNumber, str, 1);
    }
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
    char entry[SIZE_DATA];
    while (1)
    {
        if (sonConfig.opened)
        {
            readed = RS232_PollComport(sonConfig.serialNumber, readBuf, SIZE_DATA);
            

            if (readed > 0)
            {
                openFiles();
                if (readBuf[0] >= ERROR && readBuf[0] <= DATA2)
                {
                    sonConfig.iss = readBuf[1];
                    uint32_t timestamp = join32(readBuf + 2);

                    uint8_t type = readBuf[6];
                    //ERROR PACKET
                    if (readBuf[0] == ERROR)
                    {
                        sprintf(entry, "%u;%s;%s;%u;%u;\n",
                                sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, type);
                        write(fdErrors, entry, sizeof(entry));
                        if (sonConfig.realtime)
                            write(1, entry, strlen(entry));
                    }
                    else
                    {
                        //DATA 1 PACKET (LDR)
                        if (readBuf[0] == DATA1)
                            for (int j = 7; j < readed; j = j + 2)
                            {
                                if (j != 7)
                                {
                                    timestamp += (sonConfig.pa / 1000);
                                }

                                int ldr = join16(readBuf + j); //ldr

                                float voltage = ((ldr * 3.3) / (4095)); //tensao
                                float val = 5 - voltage;
                                float current = (voltage / (float)3000); //corrente
                                float converted = val / (current * 1000);

                                float lux = pow(10, ((log10(converted) - 1.7782) / -5));

                                if (checkValue('v', voltage, timestamp))
                                {
                                    //TODO: remove converted from sprintf
                                    sprintf(entry, "%u;%s;%s;%u;%c;%f;\n",
                                            sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, (char)type, lux);
                                    int n = write(fdData, entry, strlen(entry));
                                    if (sonConfig.realtime)
                                        write(1, entry, strlen(entry));
                                }
                            }
                        //DATA 2 PACKET (S)
                        else if (readBuf[0] == DATA2)
                        {
                            char state[12];
                            uint8_t value = readBuf[7];
                            if (checkValue('s', value, timestamp))
                            {
                                sonConfig.led_status = value;
                                if (value)
                                    strcpy(state, "Ligado");
                                else
                                    strcpy(state, "Desligado");

                                sprintf(entry, "%u;%s;%s;%u;%c;%s;\n",
                                        sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, (char)type, state);
                                int n = write(fdData, entry, strlen(entry));
                                if (sonConfig.realtime)
                                    write(1, entry, strlen(entry));
                            }
                        }
                    }
                }
                memset(entry, 0, sizeof entry);
                closeFiles();
            }
        }
        else
        {
            RS232_CloseComport(sonConfig.serialNumber);
            _exit(0);
        }
        sendPacket();
        usleep(100000); /* sleep for 100 milliSeconds */
    }
}

int checkValue(char type, float value, uint32_t timestamp)
{

    if (type == 'v')
    {
        if (value >= 0 && value <= 3.3)
            return 1;
    }

    if (type == 's')
    {
        if ((int)value >= 0 && (int)value <= 2)
            return 1;
    }

    char entry[SIZE_DATA];

    sprintf(entry, "%u;%s;%s;%u;%u;\n",
            sonConfig.iss, sonConfig.area, sonConfig.GPS, timestamp, BAD_VALUE_ERR);
    write(fdErrors, entry, sizeof(entry));

    return 0;
}