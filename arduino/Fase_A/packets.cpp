#include "packets.h"
#include "Arduino.h"

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
