#include "packets.h"
#include "Arduino.h"
#include "api.h"

//Identificador do sistema sensor
const char iss = 0;

/*
  return true -> Chegou ao fim
*/
bool addChar(char data1[], char v1, int pos) {
  
  data1[pos] = v1;
  pos++;
  
  if (pos == 1024) {
    return true
  }
  
  return false;
  
}


/*
  tsp -> Timestamp no momento em que se recebe o pacote Start
  pm -> Periodo de leitura de dados do sensor
  pa -> Periodo entre amostras
  na -> Número total de amostras
*/
void startPacket(char packet[], uint32_t tsp, uint32_t pm , uint32_t pa, int na ) {
  tsp = join32((char *)&packet[1]);
  pm = join32((char *)&packet[5]);
  pa = join32((char *)&packet[9]);
  na = (int)packet[13];
}

void data1Packet(char packet[] , uint32_t tsp) {
  packet[0] = 3;
  packet[1] = iss;
  split32( &packet[2] , tsp);
  packet[6] = 'V';
}

void data2Packet(char packet[] , uint32_t tsp) {
  packet[0] = 4;
  packet[1] = iss;
  split32( &packet[2] , tsp);
  packet[6] = 'S';
}

void dataPacket(char data1[], char data2[], uint32_t tsp, int pos) {
  data1Packet(data1, tsp);
  data2Packet(data2, tsp);
  pos++;
}
