  #include <SPI.h>
#include <RFM69.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not
RFM69 radio;

#define EMPTYMSG 0
#define VERIFYCODEMSG 1

#define MINLONG -2147483648L
#define MAXLONG 2147483647L

byte message[RF69_MAX_DATA_LEN];
byte messageLength;
byte senderId;

/******P I N S********/
const byte led1 = 6;
const byte led2 = 7;
const byte led3 = 8;

/*******C O D E******/ 
const byte codeLength = 8;          
char code[codeLength] = {'0','9','0','5','1','9','9','7'};

const int numTokens = 256;
long tokens[numTokens];
unsigned long tokenTime[numTokens]; 
int actualTokenIndex = 0;
unsigned long tokenValidTime = 30000;
void setup() 
{

  Serial.begin(9600);
  Serial.print("ArduinoMaster");
  radio.initialize(FREQUENCY, MASTERADDRESS, NETWORKADDRESS);
  radio.encrypt(ENCRYPTKEY);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() 
{
  if (radio.receiveDone())
  {
    copyMessage();    
    processMessage();
  }
}

void processMessage()
{
  switch(getHeader())
  {
    case VERIFYCODEMSG:
    {
      verifyCodeMsgHandler(senderId, messageLength - 1, message + 1);
      break;
    }
    default:
    {
      unknownMsgHandler(senderId, messageLength, message);
      break;
    }  
  }  
}

byte getHeader()
{
  if(messageLength > 0)
  {
    return message[0];  
  }
  return EMPTYMSG;
}

void copyMessage()
{
  senderId = radio.SENDERID;
  messageLength = radio.DATALEN;
  for (int i = 0; i < messageLength; i++)
  {
    message[i] = radio.DATA[i];
  }
  if (radio.ACKRequested())
  {
    radio.sendACK();
  }
}

void verifyCodeMsgHandler(byte senderId, byte messageLength, byte* message)
{
  /*
  3. precitat kod
  4.overit kod 
  5. vygenerovat token */
  if(verifyCode())
  {
    generateToken ();
  }  
}

void unknownMsgHandler(byte senderId, byte messageLength, byte* message)
{
  Serial.print("Neznama sprava od:");
  Serial.println(senderId);
  Serial.print("Sprava: ");
  for(byte i = 0; i < messageLength; i++)
  {
    Serial.print(message[i], HEX); 
  }
}

unsigned long generateToken ()
{
  return random(MINLONG,MAXLONG);
  
}

boolean isTokenValid(int tokenIndex)
{
  unsigned long actualTime = millis();
  unsigned long maxValidTime = tokenTime[tokenIndex];
  if(actualTime < maxValidTime)
  {
    return true;
  }
  else 
  {
    unsigned long tokenGenerationTime = maxValidTime - tokenValidTime;  
    if(tokenGenerationTime > maxValidTime)
    {
      if(actualTime>tokenGenerationTime)
      {
        return true;  
      }
    }
  }
  return false;
}

boolean verifyCode(byte readCodeLength, byte* readCode)
{
  if (codeLength != readCodeLength)
  {
    return false;   
  }
  
  for(int i=0;i < codeLength;i++)
  {
    if(readCode[i] != code[i])
    {
      return false; 
    }
  }
  return true;
}

void grantAccess()
{    
    Serial.println("Grant Access");
    digitalWrite(led1, HIGH);
    delay(180);
    digitalWrite(led2, HIGH);  
    delay(180);
    digitalWrite(led3, HIGH);
    delay(180);
    digitalWrite(led1, LOW);
    delay(180);
    digitalWrite(led2, LOW);  
    delay(180);
    digitalWrite(led3, LOW);                     
    delay(180);
} 
