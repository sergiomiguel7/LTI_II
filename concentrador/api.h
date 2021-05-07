#define CONFIG_FILE "../env/config.txt"
#define MENU_FILE "../env/menu.txt"
#define FIFO "FIFO"
#define SIZE0 32
#define SIZE1 256
#define SIZE2 512
#define SIZE3 1024
#define SIZE_DATA 127
#define MAX_SENSOR 10
#define START 0
#define STOP 1
#define ERROR 2
#define DATA1 3
#define DATA2 4
#define LED 5
#define BAD_VALUE_ERR 3
#define PORT_UDP 7777

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
long transform_data(char *date, char *hour);
int checkValue(char type, float value, uint32_t timestamp, int socketFd, int opened);

//pid server
int pidServer;

//ficheiros
int fdLogs, fdErrors, fdData;

//numero sensores ligados
int configuredPorts;

//real time
int serverPid, concentrador_id;

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
    uint32_t pa; //period between reading from sensors
    char portSerial[SIZE1]; //serial port used from arduino when write blueetooth data
    int serialNumber; //index from serial 
    int opened; //it's opened ?
    pid_t pid; //receiving data on process pid
    uint8_t iss;//sensor id
    char area[SIZE0]; //where is the sensor
    char GPS[SIZE0]; //gps coordinates from sensor
    int led_status;
    int realtime;
} config;

config actualConfig[MAX_SENSOR], sonConfig;
