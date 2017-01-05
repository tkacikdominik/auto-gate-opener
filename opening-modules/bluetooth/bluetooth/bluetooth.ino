#include <SPI.h>
#include <RFM69.h>

#define MASTERADDRESS 1
#define BLUETOOTHADDRESS 4
#define NETWORKADDRESS 0

// RFM69 frequency, uncomment the frequency of your module:

#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        true // Request ACKs or not

const char submitChar ='\n';

// Create a library object for our RFM69HCW module:
RFM69 radio;
/*****************C O D E*******************/
          /***C O N S T A T S***/
const byte maxCodeLength = 8;
          /***V A R I A B L E***/
char readCode[maxCodeLength];
byte pos;

void setup() 
{  
  Serial.begin(9600);
  resetCodePos();
  radio.initialize(FREQUENCY, BLUETOOTHADDRESS, NETWORKADDRESS);
  radio.encrypt(ENCRYPTKEY);
}

void loop() 
{
  if(Serial.available()>0)
  {
  char val=(char)Serial.read();
  Serial.println(val);  
  }
  char charRead = char(Serial.read());
  if (charRead)  
  {
    if(charRead==submitChar)
    {
      sendCodeToMaster();
      Serial.println("posielam na mastra");
    }
    else
    {
      if (pos<maxCodeLength)
      {
        readCode[pos] = charRead;
        pos++;
      }
      else
      {
        resetCode();
      }
    }
  }
}

/******M E T H O T S*********/

void resetCode()
{
  for(byte i=0;i<maxCodeLength;i++)
  {
    readCode[i]=0;  
  }
}

void resetPos()
{
  pos = 0;  
}

void resetCodePos()
{
  resetCode();
  resetPos();
}

void sendCodeToMaster()
{  
  Serial.println(readCode);
  if (radio.sendWithRetry(MASTERADDRESS, readCode, maxCodeLength, 100, 100))
  {
    
  }
  else
  {
    
  }  
  resetCodePos();
}
