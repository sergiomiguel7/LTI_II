#define ADDR "127.0.0.1"
#define PORT 8000
#define CONFIG_FILE "config.txt"
#define SIZE1 256
#define SIZE2 512
#define SIZE3 1024


//vars para socket
struct sockaddr_in servAddr, cliaddr;
int sockfd;

//socket
int sockManager;



typedef struct config {
    char portSerial[SIZE1]; //serial port used from arduino when write blueetooth data
    int serialNumber; //index from serial 
} config;

config actualConfig;
