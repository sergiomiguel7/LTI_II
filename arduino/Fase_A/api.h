#define SIZE1 256
#define SIZE2 512
#define SIZE3 1024
#define MAX_SENSOR 10
#define START 0
#define STOP 1
#define ERROR 2
#define DATA1 3
#define DATA2 4

#include<stdint.h>
#include<stdbool.h>


//utils.c functions
void split32(char *buffer, uint32_t value);
void split16(char *buffer, uint16_t value);
uint16_t join16(char *buffer);
uint32_t join32(char *buffer);
void splitFloat(char *buffer, float value);
float joinFloat(char *bufffer);


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
