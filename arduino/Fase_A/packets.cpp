#include "packets.h"
#include "Arduino.h"

void clearPacket(char packet[]) {
  memset(packet, -1 , sizeof(packet));
}


/*
  True -> Sucesso
  False -> Erro
*/
bool addChar(char packet[], char value) {
  int i = 0;
  while (packet[i] != -1) {
    i++;
    if (i > sizeof(packet))
      return false;
  }
  packet[i] = value;
  return true;
}
