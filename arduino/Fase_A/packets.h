#include "Arduino.h"

bool addInfo(char packet[], char info, int pos);

void startPacket(char packet[], uint32_t *tsp, uint32_t *pa );

void data1Packet(char packet[] , uint32_t tsp);

void data2Packet(char packet[] , uint32_t tsp, int state);

bool pirAnalysis(int state);

void errorPacket(char packet[] , uint32_t tsp, int err);

void stopPacket(char packet[], int *rsn);
