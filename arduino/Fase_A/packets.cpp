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
bool addInfo(uint8_t packet[], uint16_t info, int pos) {
  split16(&packet[pos], info);    
  if (pos > 120) {
    Serial.println((String)"Enviei um pacote!");
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
  packet[6] = 'L';
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
