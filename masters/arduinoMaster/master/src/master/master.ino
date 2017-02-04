// po dynamickom priradení sieti, nastaviť PWM pin na 0!!!

#include <GateOpenerCommunicator.h>

#define FREEADDRESS 0
#define FULLADDRESS 1

GateOpenerCommunicator communicator;
Logger logger;
Random rnd;
/******P I N S*******/
const byte led1 = 6;
const byte led2 = 7;
const byte led3 = 8;

const byte analogPin = A0;
const byte pwmPin = 3;

/*******C O D E******/ 
const byte codeLength = 8;          
char code[codeLength] = {'0','9','0','5','1','9','9','7'};

/*****T O K E N*****/ 
const int numTokens = 64;
long tokens[numTokens];
unsigned long tokenTime[numTokens]; 
int actualTokenIndex = 0;
unsigned long tokenValidTime = 30000;

byte freeAddress[32];

void setup() 
{
  logger.init();
  rnd.init(analogPin, pwmPin);
  communicator.init(MASTER, rnd, logger);
  setAddressFull(communicator.MyAddress);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  start();
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
  switch(communicator.getHeader())
  {
    case VERIFYCODEMSG:
    {
      verifyCodeMsgHandler();
      break;
    }
    case GATENUMMSG:
    {
      gateNumMsgHandler();
      break;
    }
    case REQUESTADDRESSMSG:
    {
      requestAddressMsgHandler();
      break;
    }
    default:
    {
      unknownMsgHandler();
      break;
    }  
  }  
}

boolean isAddressFree(byte addressNum)
{
  byte numOfByte = addressNum / 8;
  byte numOfBit = addressNum %8; 
  byte mask = 1 << numOfBit ;
  byte isFree = freeAddress[numOfByte];  
  return (isFree & mask)==0;    
}

void setAddressFull(byte addressNum)
{
  byte numOfByte = addressNum / 8;
  byte numOfBit = addressNum %8; 
  byte mask = 1 << numOfBit ;
  freeAddress[numOfByte] |= mask;
}

void setAddressFree(byte addressNum)
{
  byte numOfByte = addressNum / 8;
  byte numOfBit = addressNum %8; 
  byte mask = ~(1 << numOfBit) ;
  freeAddress[numOfByte] &= mask;
}

void requestAddressMsgHandler()
{
  RequestAddressMsg requestAddressMsg = RequestAddressMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(requestAddressMsg,communicator.SenderId,RECV);
  byte newAddress = getFreeAddress();
  AddressMsg addressMsg = AddressMsg(requestAddressMsg.Token, newAddress);
  logger.log(addressMsg, communicator.SenderId, SEND);
  boolean ok = communicator.reply(addressMsg);
  logger.logDeliveryStatus(ok);
  if(!ok)
  {
    setAddressFree(newAddress);
  }
}

byte getFreeAddress()
{
  for(byte i =0; i < 255; i++ )
  {
    boolean isFree = isAddressFree(i);  
    if(isFree)
    {
      setAddressFull(i);
      return i;
    }
  } 
  //Pamat pre slave je plna, treba ju cistit
  return 255;
}

void gateNumMsgHandler()
{
  GateNumMsg gateNumMsg = GateNumMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(gateNumMsg, communicator.SenderId, RECV);
  OpenGateMsg openGateMsg = OpenGateMsg();
  logger.log(openGateMsg, 255, SEND);
  boolean ok = communicator.send(255, openGateMsg);
  logger.logDeliveryStatus(ok); 
}

void verifyCodeMsgHandler()
{
  CodeMsg msg = CodeMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(msg, communicator.SenderId, RECV);
  
  if(verifyCode(msg))
  {
    int tokenIndex = generateToken();
    TokenMsg tokenMsg = TokenMsg(tokenIndex != -1, tokens[tokenIndex]);
    logger.log(tokenMsg, communicator.SenderId, SEND);    
    boolean ok = communicator.reply(tokenMsg);
    logger.logDeliveryStatus(ok);    
  }  
  else
  {
    
  }
}

void unknownMsgHandler()
{
  UnknownMsg unknownMsg = UnknownMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(unknownMsg, communicator.SenderId);
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
  long token = rnd.generateLong();
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

boolean verifyCode(CodeMsg msg)
{
  if (codeLength != msg.CodeLength)
  {
    return false;   
  }
  
  for(int i=0;i < codeLength;i++)
  {
    if(code[i] != msg.Code[i])
    {
      return false; 
    }
  }
  return true;
}

void grantAccess()
{    
    Serial.println("AG");
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

 void start()
{    
    digitalWrite(led1, HIGH);
    delay(100);
    digitalWrite(led2, HIGH);  
    delay(100);
    digitalWrite(led3, HIGH);
    delay(100);
    digitalWrite(led1, LOW);
    delay(100);
    digitalWrite(led2, LOW);  
    delay(100);
    digitalWrite(led3, LOW);                     
    delay(100);
}
