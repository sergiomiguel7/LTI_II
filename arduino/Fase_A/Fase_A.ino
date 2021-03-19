#include "BluetoothSerial.h"
#include "packets.h"


#define ERRORPACKETSIZE 9


uint32_t initialTS = 0;
uint32_t startTS = 0;
uint32_t pa = 0;
char aux [24];
char dataPacket [1024];
int pos = 0;


//O LDR está conectado com o GPIO 34
const int ldrPin = 34;

//O Led está conectado com o GPIO 25
const int ledPin = 25;

//O PIR está conectado com o GPIO 27
const int pirPin = 27;

//Estado atual do led
//0 -> desligado , 1 -> ligado
bool led = 0;


BluetoothSerial SerialBT;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  SerialBT.begin("ESP32test"); //Bluetooth device name

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);

}

void loop() {

  if (SerialBT.available()) {
    //ler trama
    SerialBT.readBytes(aux, 14);
    // verificar tipo de trama
    
    if (aux[0] == 0) {           //trama de START
      startPacket(aux, &startTS, &pa);

      bool erros = false;
      
      //procurar novos possiveis erros para enviar o errorPacket
      if (startTS <= 0) {
        //caso o startTS seja mal recebido enviamos um timeStamp default neste caso 0
        errorPacket(aux, 0, 0);
        SerialBT.write((uint8_t *)aux, ERRORPACKETSIZE);
        erros = true;
      }

      if (pa <= 0) {
        errorPacket(aux, startTS, 1);
        SerialBT.write((uint8_t *)aux, ERRORPACKETSIZE);
        erros = true;
      }

      if (!erros) {
        //obter timestamp trama e actual
        initialTS = millis();

        pos = 7;

        while (1) {
          sensorSystem();
          if (SerialBT.available()) {

            SerialBT.readBytes(aux, 6);
            if (aux[0] == 1) {
              //stopPacket(aux,rsn);          //ver parametros possiveis para a razao
              data1Packet(dataPacket, currentTimestamp());
              SerialBT.write((uint8_t *)dataPacket, pos);
              pos = 7;
            }
          }
          //O processo repete-se a cada pa ms
          delay(pa);
        }
      }
    }
  }
}


/*
  codigo quando se recebe trama start
  startTS = join32((char*)&packet[1]);
  initialTS = millis();
*/

void sensorSystem() {
  // put your main code here, to run repeatedly:
  int ldrValue = analogRead(ldrPin);
  int pirStat = digitalRead(pirPin);

  float voltage_value =  ((ldrValue * 3.3 ) / (4095));

  if (voltage_value <= 0.75) {
    digitalWrite(ledPin, HIGH);
    led = 1;
  } else {
    digitalWrite(ledPin, LOW);
    led = 0;
  }

  if (addInfo(dataPacket, voltage_value, pos)) {
    data1Packet(dataPacket, currentTimestamp());
    SerialBT.write((uint8_t *)dataPacket, pos);
    pos = 7;
  }
  //CASO HAJA MUDANCA DE ESTADO(RE FAZER PIRANALYSIS()
  if (pirAnalysis(pirStat)) {
    data2Packet(aux, currentTimestamp(), pirStat);
    SerialBT.write((uint8_t *)aux, 8);
  }
}


uint32_t currentTimestamp() {
  //calcular timestamp desde o inicio até ao enviado do concentrador
  return (millis() - initialTS) + startTS;
}
