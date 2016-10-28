#include <Key.h>
#include <Keypad.h>
#include <Wire.h>


#define MASTERADDRESS 0x01
#define CODELOCKADDRESS 0x02
/**I N I C I A L I Z A T I O N K E Y P A D**/ 
          /***C O N S T A T S***/
const char submitChar ='#';
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

/*****************C O D E*******************/
          /***C O N S T A T S***/
const byte maxCodeLength = 8;
          /***V A R I A B L E***/
char readCode[maxCodeLength];
byte pos;

void setup() 
{
  Wire.begin(CODELOCKADDRESS);
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
