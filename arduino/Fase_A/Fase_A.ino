#include "BluetoothSerial.h"
#include "packets.h"

#define ERRORPACKETSIZE 9

uint32_t initialTS = 0;
uint32_t startTS = 0;
uint32_t pa = 0;
char aux [24];
char dataPacket [1024];
int pos = 0;
/*
  Será que vale mais a pena ter um programa que lê o pacote e procura um local vazio para adicionar o novo elemento no pacote?
  Ou é melhor acrescentar uma nova variavel ao projeto que apontará sempre para o proximo local do pacote que esteja vazio?
*/

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
    SerialBT.readBytes(dataPacket, 14);
    // verificar tipo de trama
    if (dataPacket[0] == 0) {           //trama de START

      startPacket(dataPacket, &startTS, &pa);

      //obter timestamp trama e actual
      initialTS = millis();

      if(startTS <= 0){
          errorPacket(aux,&startTS,0);
          SerialBT.write(aux, ERRORPACKETSIZE);    
      }



      

    }
  }
}


/*
  codigo quando se recebe trama start
  startTS = join32((char*)&packet[1]);
  initialTS = millis();
*/

void packetConstruct() {
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

  //Envia o estado do Led por Bluetooth
  //SerialBT.write(led);
  Serial.println(voltage_value);

  //O processo repete-se a cada 100 ms
  delay(pa);
}


uint32_t currentTimestamp() {
  //calcular timestamp desde o inicio até ao enviado do concentrador
  return (millis() - initialTS) + startTS;
}
