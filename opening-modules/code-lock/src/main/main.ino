#include <Key.h>
#include <Keypad.h>


/****************************** 
***INICIALIZACIA KLAVESTNICE*** 
*******************************/ 

const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = 
{
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowsPin[rows] = {50, 51, 52, 53};
byte colsPin[cols] = {46, 47, 48, 49};
Keypad keyPad = Keypad(makeKeymap(keys), rowsPin, colsPin, rows, cols);

const char submitChar ='#';
const byte maxCodeLength = 8; 
char readCode[maxCodeLength];
byte pos;

void setup() 
{
  Serial.begin(9600);
  resetCodePos();
}

void loop() 
{
  char charRead = keyPad.getKey();
  if (charRead)
  {
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
  for(byte i=0;i<maxCodeLength;i++)
  {
     Serial.print(readCode[i]);  
  }
  Serial.println();
  resetCodePos();
}

