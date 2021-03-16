#include "Arduino.h"

bool addChar(char data1[], char data2[], char v1, char v2, int pos);

void startPacket(char packet[], uint32_t tsp, uint32_t pm , uint32_t pa, int na );

void dataPacket(char data1[], char data2[], uint32_t tsp, int pos);
