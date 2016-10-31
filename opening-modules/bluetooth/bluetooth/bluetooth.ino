#include <Wire.h>

#define MASTERADDRESS 0x01
#define BLUETOOTHADDRESS 0x03
/*****************C O D E*******************/
          /***C O N S T A T S***/
const byte maxCodeLength = 8;
const char submitChar ='\n';
          /***V A R I A B L E***/
char readCode[maxCodeLength];
byte pos;

void setup() 
{
  Wire.begin(BLUETOOTHADDRESS);
  Serial.begin(115200);
  resetCodePos();
}

void loop() 
{
  while(Serial.available() > 0)
  {
    char charRead = char(Serial.read());    
    
      if(charRead==submitChar)
      {
        sendCodeToMaster();
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
  Serial.println("Posielam na mastra");
  Serial.print(readCode);
  Serial.println();
  Wire.beginTransmission(MASTERADDRESS);
  Wire.write(readCode, maxCodeLength);
  Wire.endTransmission();
  Serial.println("Poslane!");
  resetCodePos();
}
