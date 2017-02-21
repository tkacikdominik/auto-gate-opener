#include <GateOpenerCommunicator.h>

#define COMMUNICATION 0
#define CODE 1
#define CHOOSEGATE 2

GateOpenerCommunicator communicator;
Logger logger;
Random rnd;

const byte analogPin = A5;
const byte pwmPin = 3;

const char submitChar ='\n';

          /***C O N S T A T S***/
const byte maxCodeLength = 8;
          /***V A R I A B L E***/
char readCode[maxCodeLength];
byte pos;

byte state;

void setup() 
{  
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger, NULL);
  resetCodePos();
  state = CODE;
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
      //sendCodeToMaster();
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
