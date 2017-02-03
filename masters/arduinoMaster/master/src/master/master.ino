// Generovanie Token kodu, zabezpecit pseudonahodnot 


#include <GateOpenerCommunicator.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define GATEADDRESS 3 
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not

#define MAXLONG 2147483647L

GateOpenerCommunicator communicator;
Logger logger;
/******P I N S********/
const byte led1 = 6;
const byte led2 = 7;
const byte led3 = 8;

/*******C O D E******/ 
const byte codeLength = 8;          
char code[codeLength] = {'0','9','0','5','1','9','9','7'};

const int numTokens = 64;
long tokens[numTokens];
unsigned long tokenTime[numTokens]; 
int actualTokenIndex = 0;
unsigned long tokenValidTime = 30000;

void setup() 
{
  logger.init();
  communicator.init(FREQUENCY, MASTERADDRESS, NETWORKADDRESS, ENCRYPTKEY);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  randomSeed(analogRead(0));
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
    default:
    {
      unknownMsgHandler();
      break;
    }  
  }  
}

void gateNumMsgHandler()
{
  GateNumMsg gateNumMsg = GateNumMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(gateNumMsg, communicator.SenderId, RECV);
  OpenGateMsg openGateMsg = OpenGateMsg();
  logger.log(openGateMsg, GATEADDRESS, SEND);
  boolean ok = communicator.send(GATEADDRESS, openGateMsg);
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
  long token = random(0,MAXLONG);
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
