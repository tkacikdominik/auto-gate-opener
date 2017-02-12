#include <GateOpenerCommunicator.h>
#include <MFRC522.h>

GateOpenerCommunicator communicator;
Logger logger;
Random rnd;

#define SS_PIN 5
#define RST_PIN 6

MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 

const byte cardCodeLength = 4;
byte readCode[cardCodeLength];

const byte analogPin = A0;
const byte pwmPin = 9;

void setup() 
{ 
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init();
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger);
  Serial.println("Card reader");
}
 
void loop() 
{
 readCard();
}

void printHex(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void sendCodeToMaster()
{
  Serial.println("Posielam na mastra");
  /*if (radio.sendWithRetry(MASTERADDRESS, readCode, cardCodeLength, 2, 2))
  {
    Serial.println("Prijate!");
  }
  else
  {
    Serial.println("Neprijate");
  }  */
}

void readCard()
{
    readCode[0]=0;
    if ( ! rfid.PICC_IsNewCardPresent())return;
    if ( ! rfid.PICC_ReadCardSerial())return;
    for (byte i = 0; i < 4; i++) 
    {
      readCode[i] = rfid.uid.uidByte[i];
    }
    Serial.print(F("Card code in hex: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    if(readCode[0]!=0)
    {
      sendCodeToMaster(); 
    }
}
void printDec(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
