#include <GateOpenerCommunicator.h>
#include <GateOpenerProtocol.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not

#define MINLONG -2147483648L
#define MAXLONG 2147483647L

GateOpenerCommunicator communicator = GateOpenerCommunicator(FREQUENCY, MASTERADDRESS, NETWORKADDRESS, ENCRYPTKEY);
byte messageToSend[RF69_MAX_DATA_LEN];

/******P I N S********/
const byte led1 = 6;
const byte led2 = 7;
const byte led3 = 8;

/*******C O D E******/ 
const byte codeLength = 8;          
char code[codeLength] = {'0','9','0','5','1','9','9','7'};

const int numTokens = 128;
long tokens[numTokens];
unsigned long tokenTime[numTokens]; 
int actualTokenIndex = 0;
unsigned long tokenValidTime = 30000;
void setup() 
{
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() 
{
  if (communicator.receive())
  {
    processMessage();
  }
}

void processMessage()
{
  switch(getHeader(communicator.MessageLength, communicator.Message))
  {
    case VERIFYCODEMSG:
    {
      verifyCodeMsgHandler(communicator.SenderId, communicator.MessageLength - 1, communicator.Message + 1);
      break;
    }
    default:
    {
      unknownMsgHandler(communicator.SenderId, communicator.MessageLength, communicator.Message);
      break;
    }  
  }  
}

byte getHeader(byte messageLength, byte* message)
{
  if(messageLength > 0)
  {
    return message[0];  
  }
  return EMPTYMSG;
}

void verifyCodeMsgHandler(byte senderId, byte messageLength, byte* message)
{
  Serial.print("V");
  Serial.println(senderId);
  Serial.print(" msg: ");
  for(byte i = 0; i < messageLength; i++)
  {
    Serial.print(message[i], HEX); 
  }
  if(verifyCode(messageLength, message))
  {
    int tokenIndex = generateToken();
    int messageLength = createTokenMsg(tokenIndex, messageToSend);
    sendMessage(senderId, messageToSend, messageLength);
  }  
}

int createTokenMsg(int tokenIndex, byte* buf)
{
  buf[0]=TOKENMSG;
  if(tokenIndex==-1)
  {
    buf[1]=0;
    return 2;  
  }
  else
  {
    buf[1]=1;
    longToByteArray(tokens[tokenIndex], buf, 2);
    return 6;
  }
}

void longToByteArray(long val, byte* buf, int startIndex)
{
  buf[startIndex]=(byte)val;
  buf[startIndex + 1]=(byte)(val >> 8);
  buf[startIndex + 2]=(byte)(val >> 16);
  buf[startIndex + 3]=(byte)(val >> 24);
}

void unknownMsgHandler(byte senderId, byte messageLength, byte* message)
{
  Serial.print("U");
  Serial.println(senderId);
  Serial.print(" msg: ");
  for(byte i = 0; i < messageLength; i++)
  {
    Serial.print(message[i], HEX); 
  }
}

int generateToken()
{
  actualTokenIndex++;
  actualTokenIndex %= numTokens;
  unsigned long actualTime = millis();
  if(isTokenValid(actualTokenIndex,actualTime))
  {
    return -1;
  }
  long token = random(MINLONG,MAXLONG);
  tokens[actualTokenIndex]=token;
  tokenTime[actualTokenIndex]=actualTime + tokenValidTime;
  return actualTokenIndex;
}

boolean isTokenValid(int tokenIndex, unsigned long actualTime)
{
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

void sendMessage(byte receiverId, byte* buf, int messageLength)
{
  Serial.print("S: ");
  Serial.print(receiverId);
  Serial.print(" msg: ");
  for(int i = 0;i<messageLength;i++)
  {
    Serial.print(buf[i], HEX);  
  }
  Serial.println();
  if (communicator.sendMessage(receiverId, buf, messageLength))
  {
    Serial.println(1);
  }
  else
  {
    Serial.println(0);
  }  
}

void grantAccess()
{    
    Serial.println("GA");
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
