#include "BluetoothSerial.h"
#include "packets.h"
#include "api.h"


uint32_t initialTS = 0;
uint32_t startTS = 0;
uint32_t pa = 0;
uint8_t aux [24];
uint8_t dataPacket [128];
int pos = 0;


//O LDR está conectado com o GPIO 34
const int ldrPin = 34;

//O Led está conectado com o GPIO 25
const int ledPin = 25;

//O PIR está conectado com o GPIO 27
const int pirPin = 27;

//Estado atual do led
//0 -> desligado , 1 -> ligado
bool ledC = 0;

bool var = false;

unsigned long previousTime = 0;

bool needBreak = false;

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
    if (aux[0] == START) {
      reseting();
      startPacket(aux, &startTS, &pa);
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

        int n;
        while (needBreak == false) {
          unsigned long currentTime = millis();

          if (currentTime - previousTime >= pa) {
            sensorSystem();
            previousTime = currentTime;
          }



          while (SerialBT.available() > 0) {
            uint8_t n1 = SerialBT.read();

            if (n1 == STOP) {
              int rsn;
              rsn = SerialBT.read();
              data1Packet(dataPacket, currentTimestamp());
              SerialBT.write(dataPacket, pos);
              needBreak = true;
            }

            if (n1 == LED) {
              uint8_t n2 = SerialBT.read();

              if (n2 == 0 || n2 == 1)
                ledC = n2;
              else {
                errorPacket(aux, startTS, LEDSTATERROR);
                SerialBT.write(aux, ERRORPACKETSIZE);
              }
            }
          }
        }
      }
    }
  }
}



void sensorSystem() {
  bool led;
  // put your main code here, to run repeatedly:
  int ldrValue = analogRead(ldrPin);
  int pirStat = digitalRead(pirPin);

  float voltage_value =  ((ldrValue * 3.3 ) / (4095));



  if (pirStat == HIGH) {
    if (voltage_value <= 0.75) {
      led = 1;
    } else {
      led = 0;
    }
    conditions(led);

    if (var == false) {
      data2Packet(aux, currentTimestamp(), led);
      SerialBT.write(aux, DATA2PACKETSIZE);
    }
    var = true;
  }
  else {
    digitalWrite(ledPin, LOW);
    var = false;
  }


  if (addInfo(dataPacket, voltage_value, pos)) {
    pos = pos + 4;
    size_t num = SerialBT.write(dataPacket, pos);
    data1Packet(dataPacket, currentTimestamp());
    pos = 7;
  } else {
    pos = pos + 4;
  }

}


uint32_t currentTimestamp() {
  return (millis() - initialTS) + startTS;
}

void conditions(bool led) {
  if ( led || ledC ) {
    digitalWrite(ledPin, HIGH);
    Serial.println((String) "LED 1 -> led:" + led + " ledC:" + ledC);
  }
  else {
    digitalWrite(ledPin, LOW);
    Serial.println((String) "LED 0 -> led:" + led + " ledC:" + ledC);
  }
}

void reseting() {
  previousTime = 0;
  needBreak = false;
  pos = 7;
}
