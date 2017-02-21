#include <GateOpenerCommunicator.h>

#define COMMUNICATION 0
#define CODE 1
#define CHOOSEGATE 2

GateOpenerCommunicator communicator;
Logger logger;
Random rnd;

const byte analogPin = A5;
const byte pwmPin = 3;

const char submitChar ='\n';

          /***C O N S T A T S***/
const byte maxCodeLength = 8;
          /***V A R I A B L E***/
char readCode[maxCodeLength];
byte pos;

byte state;

void setup()
{
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger, NULL);
  resetCodePos();
  state = CODE;
}

void loop()
{
  communicator.refresh();
  if(state == CODE || state == CHOOSEGATE)
  {
    if(Serial.available()>0)
    {
      char val=(char)Serial.read();
      Serial.println(val);  
    }
    char charRead = char(Serial.read());
    if (charRead)  
    {
      if(charRead==submitChar)
      {
        //sendCodeToMaster();
        Serial.println("posielam na mastra");
      }
      else
      {
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
}  
  else if (state == COMMUNICATION)
  {
    ledCommunicationOn();
    if (communicator.receive(timeoutTime))
    {
      processMessage();
    }
    else
    {
      resetState();
    }
  }
}

void resetState()
{
  invalidPip();
  goToStateCode();
}

void goToStateCode()
{
  resetCodePos();
  keyTimeout = 0;
  ledOff();
  state = CODE;  
}

void keypadEvent(KeypadEvent key)
{
  if(keypad.getState()==PRESSED)
  {
    if(state==CODE)
    {
      processKeyCode(key);
    }
    else if(state==CHOOSEGATE)
    {
      processKeyChooseGate(key);
      state = CODE;
    }
  }
}

void processKeyChooseGate(KeypadEvent key)
{  
  GateNumMsg gateNumMsg = GateNumMsg(actualToken, parseByte(key));  
  logger.log(gateNumMsg, communicator.MasterAddress, SEND);
  
  boolean ok = communicator.send(communicator.MasterAddress, gateNumMsg);
  logger.logDeliveryStatus(ok);
  if(ok)
  {
    validPip();
  }
  else
  {
    invalidPip();
  }
  goToStateCode();  
}

byte parseByte(KeypadEvent key)
{
  if(key=='1')
  {
    return 1;
  }
  if(key=='2')
  {
    return 2;
  }
  if(key=='3')
  {
    return 3;
  }
  return 4;
}

void processKeyCode(KeypadEvent key)
{
  keyTimeout = millis() + keyTimeoutTime;
  if(key=='#')
  {
    doublePip();
    sendCodeToMaster();  
  }
  else if(key=='*')
  {
    resetState();
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

void processMessage()
{
  switch(communicator.getHeader())
  {
    case TOKENMSG:
    {
      tokenMsgHandler();
      break;
    }
    default:
    {
      unknownMsgHandler();
      break;
    }  
  }  
}

void tokenMsgHandler()
{
  TokenMsg msg = TokenMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(msg, communicator.SenderId, RECV);

  if(msg.IsValid)
  {
    actualToken = msg.Token;
    state = CHOOSEGATE; 
  }
  else
  {
    invalidPip();
    state = CODE;
  }
}

void unknownMsgHandler()
{
  UnknownMsg unknownMsg = UnknownMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(unknownMsg, communicator.SenderId);
}

void sendCodeToMaster()
{
  state = COMMUNICATION;
  CodeMsg msg = CodeMsg(readCode, maxCodeLength);
  logger.log(msg, communicator.MasterAddress, SEND);
  boolean ok = communicator.send(communicator.MasterAddress, msg);
  logger.logDeliveryStatus(ok);
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

void validPip()
{
  tone(piezo, 1000, 500);
}

void invalidPip()
{
  tone(piezo, 1200, 200);
  delay(250);
  tone(piezo, 200, 200);
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

void ledCodeOn()
{
  digitalWrite(GLed, HIGH);
  //digitalWrite(RLed, LOW);
}

void ledGateOn()
{
  digitalWrite(GLed, LOW);
 //digitalWrite(RLed, HIGH);
}

void ledCommunicationOn()
{
  digitalWrite(GLed, HIGH);
 // digitalWrite(RLed, HIGH);
}

void ledOff()
{
  digitalWrite(GLed, LOW);
 // digitalWrite(RLed, LOW);
}
