#include "packets.h"
#include "api.h"

//Identificador do sistema sensor
const char iss = 0;

//Último estado do sensor PIR
int lstState = LOW;

/*
  packet[] -> Pacote na qual vai ser adicionada a informação
  info -> Iformação do tipo char a adicionar no pacote
  pos -> Posição no pacote vazia para adicionar o proximo valor

  return true -> O pacote foi preenchido
  return false -> O pacote ainda tem espaço livre
*/
bool addInfo(char packet[], char info, int pos) {
  packet[pos] = info;
  pos++;
  if (pos == 1024) {
    return true;
  }
  return false;
}


/*
  START -> [ TPM (1 byte) | TSP (4 bytes) | PA (4 bytes) ]

  tsp -> Timestamp no momento em que se recebe o pacote Start
  pa -> Periodo entre amostras
*/
void startPacket(char packet[], uint32_t *tsp, uint32_t *pa ) {
  *tsp = join32((char *)&packet[1]);
  *pa = join32((char *)&packet[5]);
}


/*
  DATA1 -> [ TPM (1 byte) | ISS (1 byte) | TSP (4 bytes) | TGM (1 byte) | VAL1 (1 byte) | VAL2 (1 byte) | VALi ... ]

  TPM -> Tipo de mensagem
  TSP -> Timestamp no momento de envio
  TGM -> Tipo de grandeza medida (S -> Estado)
  VAL -> Valor medido

  -----//-----  -----//-----  -----//-----

  packet[] -> pacote no qual vai ser adicionada a informação
  tsp -> Valor do timestamp atual
  pos -> Posição no pacote vazia para adicionar o proximo valor
*/
void data1Packet(char packet[] , uint32_t tsp) {
  packet[0] = 3;
  packet[1] = iss;
  split32(&packet[2], tsp);
  packet[6] = 'V';
}


/*
  DATA2 -> [ TPM (1 byte) | ISS (1 byte) | TSP (4 bytes) | TGM (1 byte) | VAL (1 byte) ]

  TPM -> Tipo de mensagem
  TSP -> Timestamp no momento de envio
  TGM -> Tipo de grandeza medida
  VAL -> Valor medido

  -----//-----  -----//-----  -----//-----

  packet[] -> pacote no qual vai ser adicionada a informação
  tsp -> Valor do timestamp atual
  state -> Estado do tipo inteiro, devolve 0 no caso de LOW e 1 no caso de HIGH
*/
void data2Packet(char packet[] , uint32_t tsp, int state) {
  packet[0] = 4;
  packet[1] = iss;
  split32(&packet[2] , tsp);
  packet[6] = 'S';
  packet[7] = state;
}


/*
  ERROR -> [ TPM (1 byte) | ISS (1 byte) | TSP (4 bytes) | ERR (1 byte) ]

  TPM -> Tipo de mensagem
  TSP -> Timestamp no momento de envio
  TGM -> Tipo de grandeza medida
  ERR -> Tipo de erro

  -----//-----  -----//-----  -----//-----

  packet[] -> pacote no qual vai ser adicionada a informação
  tsp -> Valor do timestamp atual
  err -> Tipo de erro ocorrido
*/
void errorPacket(char packet[] , uint32_t tsp, int err) {
  packet[0] = 2;
  packet[1] = iss;
  split32(&packet[2], tsp);
  packet[6] = err;
}


void stopPacket(char packet[], int *rsn) {
  *rsn = packet[1];
}



/*
  int state -> estado do pin (LOW / HIGH)

  return true -> O PIR alterou o seu valor
  return false -> O PIR manteve o seu valor
*/
bool pirAnalysis(int state) {
  if (state != lstState) {
    lstState = state;
    return true;
  }
  return false;
}
