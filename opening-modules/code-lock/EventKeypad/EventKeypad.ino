#include <Keypad.h>
#include <SPI.h>
#include <RFM69.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not
RFM69 radio;

const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = 
{
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowsPin[rows] = {7, 8, 9, A0};
byte colsPin[cols] = {3, 4, 5, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowsPin, colsPin, rows, cols); 

// code
const byte maxCodeLength = 8;
char readCode[maxCodeLength];
byte pos;

// piezo pin 
byte piezo = A1;

void setup(){
    Serial.begin(9600);
    keypad.addEventListener(keypadEvent);
    radio.initialize(FREQUENCY, CODELOCKADDRESS, NETWORKADDRESS);
    radio.encrypt(ENCRYPTKEY);
    resetCodePos();
    pinMode(piezo, OUTPUT);
}

void loop()
{
  if (radio.receiveDone())
  {
    Serial.print("message [");
    for (int i = 0; i < radio.DATALEN; i++)
    {
      readCode[i] = (char)radio.DATA[i];
      Serial.print((char)radio.DATA[i]);
    }
    Serial.print("], RSSI ");
    Serial.println(radio.RSSI); 
  }
  char key = keypad.getKey(); 
}

void keypadEvent(KeypadEvent key)
{
  if(keypad.getState()==PRESSED)
  {
    if(key=='#')
    {
      doublePip();
      sendCodeToMaster();  
    }
    else
    {
      pip();     
      if (pos<maxCodeLength)
      {
        readCode[pos] = key;
        pos++;
      }
      else
      {
        resetCode();
      }
    }
  }
}

/*char setMassage(char readCode)
{
  char massage[maxCodeLength];
  for(byte i=0;i<2;i++)
  {
    massage[i]=statusProcess[i];
  }
  for(byte i=2;i<maxCodeLength-2;i++)
  {
    massage[i]=readCode[i-2];
  }
}

char getMassage(char readCode)
{
  char massage[maxCodeLength];
  for(byte i=2;i<maxCodeLength;i++)
  {
    massage[i-2]=readCode[i]; 
  }
  return massage;
}

char getCode(char readCode)
{
  char code[2];
  for(byte i=0;i<2;i++)
  {
    code[i]=readCode[i]; 
  }
  return code;
}

void processCode(char readCode)
{
  switch(getCode[readCode])
  {
    case '01':
    if(getMassage[readCode]==1)
    {
      //pip 4 krat
      //setstatus 02 
    }
    else
    {
      //resedCode
      //00
    }
    break;
    case '02':
    
    break;    
  }
}*/

void sendCodeToMaster()
{
  Serial.println("Posielam na mastra");
  Serial.println(readCode);
  if (radio.sendWithRetry(MASTERADDRESS, readCode, maxCodeLength, 3, 100))
  {
    Serial.println("Prijate!");
  }
  else
  {
    Serial.println("Neprijate");
  }  
  resetCodePos();
}

void pip()
{
  tone(piezo, 1000, 100); 
}

void doublePip()
{
  tone(piezo, 1200, 200);
  delay(250);
  tone(piezo, 1200, 200);
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
