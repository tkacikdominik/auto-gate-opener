                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               #include <GateOpenerCommunicator.h>
#include <MFRC522.h>

#define READCARD 1
#define SENDCARDID 2


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

byte state;

void setup() 
{ 
  SPI.begin(); 
  rfid.PCD_Init();
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger, NULL);
  state = READCARD;
}
 
void loop() 
{
  switch(state)
  {
    case READCARD:readCard();break;
    case SENDCARDID:sendCardIdToMaster();break;
  }
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
      state = SENDCARDID;
    }
}

void sendCardIdToMaster()
{
  CardMsg msg = CardMsg(readCode, cardCodeLength);
  logger.log(msg, communicator.MasterAddress, SEND);
  boolean ok = communicator.send(communicator.MasterAddress, msg);
  logger.logDeliveryStatus(ok);
  resetCardId();
}

void resetCardId()
{
  for(byte i=0;i<cardCodeLength;i++)
  {
    readCode[i]=0;  
  }
}

void printDec(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void printHex(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
