#include "Arduino.h"
#define ERRORPACKETSIZE 9
#define STOPPACKETSIZE 6
#define DATA2PACKETSIZE 8
#define ISS 0


bool addInfo(uint8_t packet[], uint8_t info, int pos);

void startPacket(uint8_t packet[], uint32_t *tsp, uint32_t *pa );

void data1Packet(uint8_t packet[] , uint32_t tsp);

void data2Packet(uint8_t packet[] , uint32_t tsp, int state);

bool pirAnalysis(int state);

void errorPacket(uint8_t packet[] , uint32_t tsp, int err);

void stopPacket(uint8_t packet[], int *rsn);
