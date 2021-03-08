#define ADDR "127.0.0.1"
#define PORT 8000
#define CONFIG_FILE "../env/config.txt"
#define SIZE1 256
#define SIZE2 512
#define SIZE3 1024
#define START 0
#define STOP 1
#define ERROR 2
#define DATA1 3
#define DATA2 4

#include<stdint.h>
#include<stdbool.h>


//utils.c functions
uint32_t current_timestamp();
void split32(char *buffer, uint32_t value);
void split16(char *buffer, uint16_t value);
uint16_t join16(char *buffer);
uint32_t join32(char *buffer);
void splitFloat(char *buffer, float value);
float joinFloat(char *bufffer);

//vars para socket
//struct sockaddr_in servAddr, cliaddr;
//int sockfd;

//socket
int sockManager;

//ficheiros
int fdLogs, fdErrors, fdData;

//estruturas para conversao de bytes
typedef union
{
 float number;
 uint8_t bytes[4];
} FLOATUNION_t;

typedef union
{
 uint16_t number;
 uint8_t bytes[2];
} UINT16UNION_t;

typedef union
{
 uint32_t number;
 uint8_t bytes[4];
} UINT32UNION_t;


typedef struct config {
    uint32_t pm; //period between sending DATA 
    uint32_t pa; //period between reading from sensors
    uint8_t na; //numero de amostras
    char portSerial[SIZE1]; //serial port used from arduino when write blueetooth data
    int serialNumber; //index from serial 
} config;

config actualConfig;
