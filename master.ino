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
const byte cardLength = 11; 
const byte countOfCard = 4; 
char cardCode[countOfCard][cardLength] = 
{
{'1','9','6','5','1','1','9','8','3','6','1'},
{'1','1','6','1','0','9','2','0','1','3','1'},
{'1','5','9','8','0','1','4','7','2','5','2'},
{'2','0','1','0','3','2','0','0','3','6','2'}
};
const byte chipLength = 10; 
const byte countOfChip = 1; 
char chipCode[countOfCard][cardLength]=
{
  {'2','4','5','6','3','2','1','6','5','1'}
};
char cardNum;
/*****T O K E N*****/ 
const int numTokens = 64;
long tokens[numTokens];
unsigned long tokenTime[numTokens]; 
int actualTokenIndex = 0;
unsigned long tokenValidTime = 30000;

/*****G A T E S*****/ 

const byte numOfGate = 16; 
byte gates[numOfGate][2];
byte gateAddress;
byte gateId;


byte freeAddress[32];

void setup() 
{
  logger.init();
  rnd.init(analogPin, pwmPin);
  communicator.init(MASTER, rnd, logger, NULL);
  setAddressFull(communicator.MyAddress);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  start();
  clearGates();
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
    case GATEIDMSG:
    {
      gateIdMsgHandler();
      break;
    }
    case PINGMSG:
    {
      pingMsgHandler();
      break;
    }
    case CARDMSG:
    {
      cardMsgHandler();
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

void cardMsgHandler()
{
  byte gateAddressHelper;
  CardMsg cardMsg = CardMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(cardMsg, communicator.SenderId, RECV);
  boolean ok = verifyCard(cardMsg)^verifyChip(cardMsg);
  if(ok)
  {
    GateNumMsg gateNumMsg = GateNumMsg(communicator.RecvMessage, communicator.MessageLength);
    logger.log(gateNumMsg, communicator.SenderId, RECV);
    for(byte i=0; i<numOfGate; i++)
    {
      Serial.println(cardNum);
      Serial.println(gates[i][1]);
      if(cardNum == gates[i][1])
      {
      gateAddressHelper = gates[i][0];
      OpenGateMsg openGateMsg = OpenGateMsg();
      logger.log(openGateMsg, gateAddressHelper, SEND);
      boolean ok = communicator.send(gateAddressHelper, openGateMsg);
      logger.logDeliveryStatus(ok); 
      break;
      }
    }
  }
  else
  {
    //neplatnakarta
  }
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

void gateIdMsgHandler()
{
  GateIdMsg gateIdMsg = GateIdMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(gateIdMsg,communicator.SenderId,RECV);
  gateAddress = communicator.SenderId;
  gateId = gateIdMsg.GateId;
  for(byte i = 0; i< numOfGate; i++)
  {
    if(gates[i][1]==gateId)
    {
      gates[i][0] = gateAddress;
      gates[i][1] = gateId;
      break;
    }
    else if(gates[i][0]==100)
    {
      gates[i][0] = gateAddress;
      gates[i][1] = gateId;
      break;
    }

  }
}

byte getFreeAddress()
{
  for(byte i =0; i < 254; i++ )
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
  byte gateAddressHelper;
  GateNumMsg gateNumMsg = GateNumMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(gateNumMsg, communicator.SenderId, RECV);
  for(byte i=0; i<numOfGate; i++)
  {
    if(gateNumMsg.GateId == gates[i][1])
    {
      gateAddressHelper = gates[i][0];
      OpenGateMsg openGateMsg = OpenGateMsg();
      logger.log(openGateMsg, gateAddressHelper, SEND);
      boolean ok = communicator.send(gateAddressHelper, openGateMsg);
      logger.logDeliveryStatus(ok);
      break;
    }
  }
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

void pingMsgHandler()
{
  PingMsg pingMsg = PingMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(pingMsg,communicator.SenderId,RECV);
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
  
  for(int i = 0; i < codeLength; i++)
  {
    if(code[i] != msg.Code[i])
    {
      return false; 
    }
  }
  return true;
}

boolean verifyCard(CardMsg msg)
{
  byte i = 0;
  if (cardLength-1 != msg.CodeLength)
  {
    return false;   
  }
  for(byte x = 0;x < countOfCard; x++)
  {
    for(i = 0;i < cardLength-1; i++)
    {
      if(cardCode[x][i] != msg.CardCode[i])
      {
        if(x==countOfCard-1)
        {
          return false;
        }
        break;
      }
    }
    if(i==cardLength-1)
    {
      cardNum = cardCode[x][cardLength-1];
      return true;
    }
  }
}

boolean verifyChip(CardMsg msg)
{
  byte i = 0;
  if (chipLength != msg.CodeLength)
  {
    return false;   
  }
  for(byte x = 0;x < countOfChip; x++)
  {
    for(i = 0;i < chipLength-1; i++)
    {
      if(chipCode[x][i] != msg.CardCode[i])
      {
        if(x==countOfChip-1)
        {
          return false;
        }
        break;
      }
    }
    if(i==chipLength-1)
    {
      cardNum = chipCode[x][chipLength-1];
      return true;
    }
  }
}

void clearGates()
{
for(byte i = 0;i<16;i++)
  {
    gates[i][0] = 100;
  }
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
