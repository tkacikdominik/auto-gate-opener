#include <SPI.h>
#include <RFM69.h>

#define MASTERADDRESS 3
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not
RFM69 radio;


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

void setup() 
{

  Serial.begin(9600);
  radio.initialize(FREQUENCY, MASTERADDRESS, NETWORKADDRESS);
  radio.encrypt(ENCRYPTKEY);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  resetCode();
}

void loop() 
{
 // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:
  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:
    for (byte i = 0; i < radio.DATALEN; i++)
    {
      Serial.print((char)radio.DATA[i]);
      readCode[i]=radio.DATA[i]; 
    }
    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
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

boolean verifyCode()
{
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

