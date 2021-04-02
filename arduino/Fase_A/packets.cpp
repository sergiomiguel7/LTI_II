#include "packets.h"
#include "api.h"

//Último estado do sensor PIR
int lstState = LOW;

/*
  packet[] -> Pacote na qual vai ser adicionada a informação
  info -> Iformação do tipo uint8_t a adicionar no pacote
  pos -> Posição no pacote vazia para adicionar o proximo valor

  return true -> O pacote foi preenchido
  return false -> O pacote ainda tem espaço livre
*/
bool addInfo(uint8_t packet[], float info, int pos) {
  splitFloat(&packet[pos], info);    
  Serial.println((String)pos +" ->->-> " + info + " >>>> " + packet[pos] + " " + packet[pos + 1] + " " + packet[pos + 2] + " " + packet[pos + 3]);
  if (pos > 120) {
    Serial.println((String)"Cheguei Aqui!!!!");
    return true;
  }
  return false;
}


/*
  START -> [ TPM (1 byte) | TSP (4 bytes) | PA (4 bytes) ]

  tsp -> Timestamp no momento em que se recebe o pacote Start
  pa -> Periodo entre amostras
*/
void startPacket(uint8_t packet[], uint32_t *tsp, uint32_t *pa ) {
  *tsp = join32(&packet[1]);
  *pa = join32(&packet[5]);
}


/*
  DATA1 -> [ TPM (1 byte) | ISS (1 byte) | TSP (4 bytes) | TGM (1 byte) | VAL1 (4 bytes) | VAL2 (4 bytes) | VALi ... ]

  TPM -> Tipo de mensagem
  TSP -> Timestamp no momento de envio
  TGM -> Tipo de grandeza medida (S -> Estado)
  VAL -> Valor medido

  -----//-----  -----//-----  -----//-----

  packet[] -> pacote no qual vai ser adicionada a informação
  tsp -> Valor do timestamp atual
  pos -> Posição no pacote vazia para adicionar o proximo valor
*/
void data1Packet(uint8_t packet[] , uint32_t tsp) {
  packet[0] = DATA1;
  packet[1] = ISS;
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
void data2Packet(uint8_t packet[] , uint32_t tsp, int state) {
  packet[0] = DATA2;
  packet[1] = ISS;
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
void errorPacket(uint8_t packet[] , uint32_t tsp, int err) {
  packet[0] = ERR;
  packet[1] = ISS;
  split32(&packet[2], tsp);
  packet[6] = err;
}


void stopPacket(uint8_t packet[], int *rsn) {
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
