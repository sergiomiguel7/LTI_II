#include "BluetoothSerial.h"
#include "packets.h"
#include "api.h"


uint32_t initialTS = 0;
uint32_t startTS = 0;
uint32_t pa = 0;
uint8_t aux [24];
uint8_t dataPacket [235];
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

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);

}

void loop() {

  if (SerialBT.available()) {
    //ler trama
    SerialBT.readBytes(aux, STARTPACKETSIZE);
    // verificar tipo de trama

    if (aux[0] == START) {           //trama de START
      Serial.println("Recebi pacote Start");
      startPacket(aux, &startTS, &pa);
      Serial.println((String)startTS + " -||- " + pa );
      data1Packet(dataPacket, currentTimestamp());


      bool erros = false;

      //procurar novos possiveis erros para enviar o errorPacket
      if (startTS <= 0) {
        //caso o startTS seja mal recebido enviamos um timeStamp default neste caso 0
        errorPacket(aux, 0, TSERROR);
        SerialBT.write(aux, ERRORPACKETSIZE);
        erros = true;
      }

      if (pa <= 0) {
        errorPacket(aux, startTS, PAERROR);
        SerialBT.write(aux, ERRORPACKETSIZE);
        erros = true;
      }

      if (!erros) {
        //obter timestamp trama e actual
        initialTS = millis();

        pos = 7;

        while (1) {
          sensorSystem();
          if (SerialBT.available()) {

            SerialBT.readBytes(aux, STOPPACKETSIZE);
            if (aux[0] == STOP) {
              stopPacket(aux,0);          //ver parametros possiveis para a razao
              data1Packet(dataPacket, currentTimestamp());
              SerialBT.write(dataPacket, pos);
              pos = 7;
              break;
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
  startTS = join32((uint8_t*)&packet[1]);
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
    SerialBT.write(dataPacket, pos);
    data1Packet(dataPacket, currentTimestamp());
    pos = 7;
  } else {
    pos = pos + 4;
  }
  //CASO HAJA MUDANCA DE ESTADO(RE FAZER PIRANALYSIS()
  if (pirAnalysis(pirStat)) {
    data2Packet(aux, currentTimestamp(), pirStat);
    SerialBT.write(aux, DATA2PACKETSIZE);
  }

}


uint32_t currentTimestamp() {
  //calcular timestamp desde o inicio até ao enviado do concentrador
  return (millis() - initialTS) + startTS;
}
