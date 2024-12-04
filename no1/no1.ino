#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8
#define LED_VERDE 3
#define LED_VERMELHO 4

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
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.startListening();
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
}

void loop()
{
  int umidade = random(0, 101); // random umidade

  if (umidade <= 60)
  {
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(LED_VERDE, LOW);
  }
  else
  {
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_VERDE, HIGH);
  }

  // Estrutura da mensagem: [tipo msg, valor da umidade]
  int mensagem[2] = {MSG, umidade};

  // envia a mensagem e o valor da umidade
  radio.stopListening();
  bool report = radio.write(&mensagem, sizeof(mensagem));
  radio.startListening();

  if (report)
  {
    Serial.print(F("MSG enviada com sucesso! Umidade enviada: "));
    Serial.println(umidade);

    // Espera o ACK
    unsigned long inicio = millis();
    bool ackRecebido = false;
    while (millis() - inicio < 500)
    {
      if (radio.available())
      {
        int resposta;
        radio.read(&resposta, sizeof(resposta));

        if (resposta == ACK)
        {
          Serial.println(F("ACK recebido!"));
          ackRecebido = true;
          break;
        }
      }
    }

    if (!ackRecebido)
    {
      Serial.println(F("Nenhum ACK recebido."));
    }
  }
  else
  {
    Serial.println(F("Falha na transmissão"));
  }

  delay(5000);
}