#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8
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
}

void loop()
{
  radio.stopListening(); // Para de ouvir/envia o cts
  int cts = CTS;
  bool ctsEnviado = radio.write(&cts, sizeof(cts));
  radio.startListening(); // volta a ouvir (umidade)

  if (ctsEnviado)
  {
    Serial.println(F("CTS enviado, pronto para receber a umidade."));
  }
  else
  {
    Serial.println(F("Falha ao enviar CTS."));
  }

  // verifica se recebeu
  unsigned long inicio = millis();
  bool umidadeRecebida = false;

  while (millis() - inicio < 1000)
  {
    if (radio.available())
    {
      int mensagem[2];
      radio.read(&mensagem, sizeof(mensagem));

      if (mensagem[0] == MSG)
      {
        int umidade = mensagem[1];
        umidadeRecebida = true;

        // Envia o ACK de volta
        radio.stopListening();
        int ack = ACK;
        radio.write(&ack, sizeof(ack));
        radio.startListening();

        Serial.print(F("Umidade recebida: "));
        Serial.println(umidade);
        break;
      }
    }
  }

  if (!umidadeRecebida)
  {
    Serial.println(F("Nenhuma mensagem de umidade recebida."));
  }

  delay(5000);
}