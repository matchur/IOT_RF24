#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8
#define LED_VERDE 3
#define LED_VERMELHO 4

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3

RF24 radio(CE_PIN, CSN_PIN);
uint64_t address = 0xF0F0F0F0E1LL;

void setup()
{
  Serial.begin(115200);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(100);
  radio.setDataRate(RF24_2MBPS);
  radio.setAutoAck(false);
  radio.openReadingPipe(1, address);
  radio.openWritingPipe(address);
  radio.startListening();
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
}

void loop()
{
  if (radio.available())
  {
    Serial.println(F("Dados disponíveis para leitura"));

    // Envia CTS e espera para receber
    int cts = CTS;
    radio.stopListening();
    radio.write(&cts, sizeof(cts));
    radio.startListening();

    delay(100);

    if (radio.available())
    {
      Serial.println(F("Mensagem disponível para leitura"));
      int mensagem[2];
      radio.read(&mensagem, sizeof(mensagem));

      if (mensagem[0] == MSG)
      {
        int umidade = mensagem[1];

        Serial.print(F("Umidade recebida: "));
        Serial.println(umidade);

        if (umidade <= 60)
        {
          digitalWrite(LED_VERMELHO, HIGH);
          digitalWrite(LED_VERDE, LOW);
          Serial.println(F("Umidade baixa! LED vermelho aceso, ligando a bomba."));
        }
        else
        {
          digitalWrite(LED_VERMELHO, LOW);
          digitalWrite(LED_VERDE, HIGH);
          Serial.println(F("Umidade alta! LED verde aceso, desligando a bomba."));
        }

        // Envia ACK e confirma recebimento
        int ack = ACK;
        radio.stopListening();
        radio.write(&ack, sizeof(ack));
        radio.startListening();
      }
    }
    else
    {
      Serial.println(F("Nenhuma mensagem recebida após CTS"));
    }
  }
  else
  {
    Serial.println(F("Nenhum dado disponível"));
  }

  delay(5000);
}