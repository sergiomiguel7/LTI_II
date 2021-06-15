#include "BluetoothSerial.h"
#include "packets.h"
#include "api.h"
#include "math.h"
#include "EspMQTTClient.h"
const String TOPICPUB = "room/mov"; // Topic to publish to
const String TOPICSUB = "room/light/" + String(ISS); // Topic to subcribe to

uint32_t initialTS = 0;
uint32_t startTS = 0;
uint32_t pa = 0;
uint8_t aux [24];
uint8_t dataPacket[128];
int pos = 0;

EspMQTTClient client(
  "-----",
  "-----",
  "192.168.-.-",  // MQTT Broker server ip
  "ISS_1",    // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

//O LDR está conectado com o GPIO 34
const int ldrPin = 34;

//O Led está conectado com o GPIO 25
const int ledPin = 25;

//O PIR está conectado com o GPIO 27
const int pirPin = 27;

//Estado atual do led
//0 -> desligado , 1 -> ligado, 2 -> dependente
int ledC = 2;

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
  Serial.println(TOPICSUB);
  Serial.println(TOPICPUB);
}

void onConnectionEstablished()
{
  client.subscribe(TOPICSUB, [](const String & payload) {
    Serial.println("Mensagem Recebida: Tópico: \"" + TOPICSUB + "\"; Payload: " + messag);
    ledC = payload.toInt();
    switch (ledC) {
      case 0:
        digitalWrite(ledPin, LOW);
        break;
      case 1:
        digitalWrite(ledPin, HIGH);
        break;
      case 2:
        break;
    }
  });
}

void loop() {
  client.loop();
    
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
          client.loop();
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

              ledC = n2;
              Serial.println((String)"N2 ->" + n2);
              switch (ledC) {
                case 0:
                  digitalWrite(ledPin, LOW);
                  break;
                case 1:
                  digitalWrite(ledPin, HIGH);
                  break;
                case 2:
                  break;
                default:
                  errorPacket(aux, startTS, LEDSTATERROR);
                  SerialBT.write(aux, ERRORPACKETSIZE);
                  break;
              }
            }
          }
        }
      }
    }
  }
}



void sensorSystem() {
  int led;
  // put your main code here, to run repeatedly:
  uint16_t ldrValue = analogRead(ldrPin);
  int pirStat = digitalRead(pirPin);

  float voltage_value = ((ldrValue * 3.3) / (4095)); //
  float val = 5 - voltage_value;
  float current = (voltage_value / (float)3000); //
  float converted = val / (current * 1000);
  float lux = pow(10, ((log10(converted) - 1.7782) / -5));

  if (pirStat == HIGH) {
    if (voltage_value <= 2.0) {
      led = 1;
    } else {
      led = 0;
    }
    led = conditions(led);

    if (var == false) {
      String messag = String(ISS) + ";" + String(currentTimestamp() + ";" + String(led));
      Serial.println("Mensagem Publicada: Tópico: \"" + TOPICPUB + "\"; Payload: " + messag);
      client.publish(TOPICPUB, messag);
    }
    var = true;
  }
  else {
    if (ledC == 2)
      digitalWrite(ledPin, LOW);
    var = false;
  }


  if (addInfo(dataPacket, ldrValue, pos)) {
    pos = pos + 2;
    size_t num = SerialBT.write(dataPacket, pos);
    data1Packet(dataPacket, currentTimestamp());
    pos = 7;
  } else {
    pos = pos + 2;
  }

}


uint32_t currentTimestamp() {
  Serial.println((String)"Millis: " + millis() + " InitialTS: " + initialTS + " startTS: " + startTS);
  return ((uint32_t)((millis() - initialTS) / 1000) + startTS);
}

int conditions(bool led) {
  if (ledC == 2) {
    if ( led ) {
      digitalWrite(ledPin, HIGH);
      return 1;
    }
    else {
      digitalWrite(ledPin, LOW);
      return 0;
    }
  }
  else
    return ledC;
}

void reseting() {
  previousTime = 0;
  needBreak = false;
  pos = 7;
}
