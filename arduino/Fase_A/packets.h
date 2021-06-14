#include "Arduino.h"
#define ERRORPACKETSIZE 7
#define STOPPACKETSIZE 2
//#define DATA2PACKETSIZE 8
#define STARTPACKETSIZE 9
#define LEDPACKETSIZE 2
#define TSERROR 0
#define PAERROR 1
#define LEDSTATERROR 2
#define ISS 1


bool addInfo(uint8_t packet[], uint16_t info, int pos);

void startPacket(uint8_t packet[], uint32_t *tsp, uint32_t *pa );

void data1Packet(uint8_t packet[] , uint32_t tsp);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void data2Packet(uint8_t packet[] , uint32_t tsp, int state);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool pirAnalysis(int state);

void errorPacket(uint8_t packet[] , uint32_t tsp, int err);
