#include "packets.h"
#include "Arduino.h"
#include "api.h"

/*
  packet[] -> Pacote á qual vai ser adiciona o value
  value -> conteudo do  tipo char que irá ser adicionado ao pacote
  pos -> apontador para a proxima posição livre no pacote
*/
void addChar(char packet[], char value, int pos) {
  if (pos < 1028) {
    packet[pos] = value;
    pos++;
  }
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
