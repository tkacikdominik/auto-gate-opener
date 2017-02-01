#include <GateOpenerCommunicator.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not

#define MINLONG -2147483648L
#define MAXLONG 2147483647L

GateOpenerCommunicator communicator = GateOpenerCommunicator(FREQUENCY, MASTERADDRESS, NETWORKADDRESS, ENCRYPTKEY);

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
  switch(communicator.getHeader())
  {
    case VERIFYCODEMSG:
    {
      verifyCodeMsgHandler();
      break;
    }
    default:
    {
      unknownMsgHandler();
      break;
    }  
  }  
}

void verifyCodeMsgHandler()
{
  CodeMsg msg = CodeMsg(communicator.RecvMessage, communicator.MessageLength);
  printCodeMsg(msg);
  
  if(verifyCode(msg))
  {
    int tokenIndex = generateToken();
    TokenMsg tokenMsg = TokenMsg(tokenIndex != -1, tokens[tokenIndex]);
    printTokenMsg(tokenMsg);    
    boolean ok = communicator.reply(tokenMsg);
    printDeliveryStatus(ok);    
  }  
}

void printDeliveryStatus(boolean ok)
{
    if (ok)
    {
      Serial.println("Delivered");
    }
    else
    {
      Serial.println("Not delivered");
    }  
}

void printCodeMsg(CodeMsg codeMsg)
{
  Serial.print("C ");
  for(byte i = 0; i < codeMsg.CodeLength; i++)
  {
    Serial.print((char)codeMsg.Code[i]);
  }
  Serial.print("(");
  Serial.print(codeMsg.CodeLength);
  Serial.print(") ");
  Serial.print("[");
  Serial.print(communicator.SenderId);
  Serial.println("]");
}

void printTokenMsg(TokenMsg tokenMsg)
{
  Serial.print("T ");
  Serial.print(tokenMsg.IsValid);
  Serial.print(" (");
  Serial.print(tokenMsg.Token);
  Serial.print(") ");
  Serial.print("[");
  Serial.print(communicator.SenderId);
  Serial.println("]");
}

void unknownMsgHandler()
{
  Serial.print("U");
  Serial.println(communicator.SenderId);
  Serial.print(" msg: ");
  for(byte i = 0; i < communicator.MessageLength; i++)
  {
    Serial.print(communicator.RecvMessage[i], HEX); 
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
