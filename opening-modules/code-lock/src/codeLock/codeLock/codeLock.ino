#include <SPI.h>
#include <RFM69.h>
//#include <Key.h>
#include <Keypad.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not
RFM69 radio;

//keypad
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
byte rowsPin[rows] = {7, 8, 9, A0};
byte colsPin[cols] = {3, 4, 5, 6};
Keypad keyPad = Keypad(makeKeymap(keys), rowsPin, colsPin, rows, cols); 

// code
const byte maxCodeLength = 8;
char readCode[maxCodeLength];
byte pos;

// piezo pin 
byte piezo = A1;

// checkEnergy 
byte pinVoltage = A2;
boolean conditionBattery;
byte lowBattery = 3;//Volt

void setup() 
{  
  Serial.begin(9600);
  resetCodePos();
  radio.initialize(FREQUENCY, CODELOCKADDRESS, NETWORKADDRESS);
  radio.encrypt(ENCRYPTKEY);
  pinMode(piezo, OUTPUT);
}

void loop() 
{
  checkVoltage();
  char charRead = keyPad.getKey();
  if (charRead)
  {
    if(charRead==submitChar)
    {    
      doublePip();
      sendCodeToMaster();
    }
    else
    {
      pip();     
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

// F U N C T I O N
boolean checkVoltage()
{
  int measuredVoltage = analogRead(pinVoltage);
  float voltage = measuredVoltage * (5.0 / 1023.0);
  if(voltage<lowBattery)
  {
    conditionBattery = true;
  }
  else
  {
    conditionBattery = false;
  }
  return conditionBattery;
}

// M E T H O T S

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

void sendCodeToMaster()
{
  Serial.println("Posielam na mastra");
  Serial.println(readCode);
  if (radio.sendWithRetry(MASTERADDRESS, readCode, maxCodeLength, 2, 2))
  {
    Serial.println("Prijate!");
  }
  else
  {
    Serial.println("Neprijate");
  }  
  resetCodePos();
}

void receiveDataOFMaster()
{
  if (radio.receiveDone())
  {
    Serial.println("massage[");
    for (byte i = 0; i < radio.DATALEN; i++)
    {
      Serial.print((char)radio.DATA[i]);
    }
    Serial.print("]");
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
  }
}

