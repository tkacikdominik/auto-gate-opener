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

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  char charRead = keyPad.getKey();
  if (charRead)
  {
    Serial.println(charRead);
  }
}
