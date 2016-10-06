#include <Key.h>
#include <Keypad.h>

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
const char emptySymbol = 'x';
          /***V A R I A B L E***/
char readCode[maxCodeLength];
byte pos;
char code[maxCodeLength] = {'0','9','0','5','1','9','9','7'};

/******************G A T E*****************/ 
          /***C O N S T A T S***/
const byte relay1 = 11;
const byte relay2 = 12;

void setup() 
{
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
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
  Serial.print(readCode);
  if(verifyCode())
  {
    digitalWrite(relay2, HIGH);
    delay(2000);
    digitalWrite(relay2, LOW);
  }
 Serial.println();
  resetCodePos();
}

boolean verifyCode()
{
  boolean check = true;
  for(byte i=0;i<maxCodeLength;i++)
  {
    if(readCode[i] != code[i])
    {
      return false;   
    }
  }
  return check;
}
