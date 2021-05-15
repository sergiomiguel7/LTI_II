#define SA struct sockaddr
#define PORT_UDP    7777
#define PORT_TCP    7778
#define MAXLINE 1024
#define MAX 128
#define START_TCP 1
#define STOP_TCP 2
#define DATA_TCP 3
#define ERRO_TCP 4
#define END_TCP 5
#define AUTH_FAIL 1

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