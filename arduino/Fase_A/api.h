#define SIZE1 256
#define SIZE2 512
#define SIZE3 1024
#define MAX_SENSOR 10
#define START 0
#define STOP 1
#define ERR 2
#define DATA1 3
#define DATA2 4
#define LED 5

#include "Arduino.h"
#include "stdint.h"
#include "stdbool.h"


//utils.c functions
void split32(uint8_t *buffer, uint32_t value);
void split16(uint8_t *buffer, uint16_t value);
uint16_t join16(uint8_t *buffer);
uint32_t join32(uint8_t *buffer);
void splitFloat(uint8_t *buffer, float value);
float joinFloat(uint8_t *bufffer);


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
