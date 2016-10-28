#include<Wire.h>

/******P I N S********/
const byte relay1 = 11;
const byte relay2 = 12;

/*******C O D E******/ 
const byte maxCodeLength = 8;
const char emptySymbol = 'x';
          
char readCode[maxCodeLength];
char code[maxCodeLength] = {'0','9','0','5','1','9','9','7'};

/**V A R I A B L E**/
boolean check = true;

#define MASTERADDRESS 0x01
#define CODELOCKADDRESS 0x02

void setup() 
{
  Wire.begin(MASTERADDRESS);
  Wire.onReceive(receiveI2C);
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  resetCode();
}

void loop() 
{

}

void receiveI2C(int bytesCount)
{
  Serial.println("Niekto mi posiela data:");
  byte readCount = 0;
  resetCode();
  while(Wire.available() > 0)
  {
    char r = Wire.read();
    Serial.print(r);
    if(maxCodeLength > readCount)
    {
      readCode[readCount] = r;
      readCount++;  
    }
  } 
  Serial.println();
  Serial.println("Koniec primania");
  
  if(verifyCode())
  {
    Serial.println("Otvaram");
    grantAccess();
  }
  else
  {
    Serial.println("Neotvaram");
  }
}

void resetCode()
{
  for(byte i=0;i<maxCodeLength;i++)
  {
    readCode[i]=0;  
  }
}

boolean verifyCode()
{
  Serial.println("Overujem");
  for(byte i=0;i<maxCodeLength;i++)
  {
    if(readCode[i] != code[i])
    {
      Serial.println("ZLE");
      return false;   
      resetCode();
    }
    Serial.println("OK");
  }
  return check;
  resetCode();
}

void grantAccess()
{    
    digitalWrite(relay2, HIGH);
    delay(2000);
    digitalWrite(relay2, LOW);  
}

