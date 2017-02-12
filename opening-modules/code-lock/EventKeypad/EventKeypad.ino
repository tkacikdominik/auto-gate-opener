#include <GateOpenerCommunicator.h>
#include <Keypad.h>

#define COMMUNICATION 0
#define CODE 1
#define CHOOSEGATE 2

const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = 
{
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowsPin[rows] = {A0, A1, A2, A3};
byte colsPin[cols] = {A4, 5, 6, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowsPin, colsPin, rows, cols); 

GateOpenerCommunicator communicator;
Logger logger;
Random rnd;

// code
const byte maxCodeLength = 8;
char readCode[maxCodeLength];
byte pos;
unsigned long keyTimeout = 0;

// piezo pin 
byte piezo = 9;
byte GLed = 8;
//byte RLed = A5;
const byte analogPin = A5;
const byte pwmPin = 3;

byte state;
long actualToken;
const unsigned long timeoutTime = 5000;
const unsigned long keyTimeoutTime = 3000;

void setup()
{
  ledCommunicationOn();
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger);
  pinMode(piezo, OUTPUT);
 // pinMode(RLed, OUTPUT);
  pinMode(GLed, OUTPUT);
  keypad.addEventListener(keypadEvent);
  resetCodePos();  
  state = CODE;
  ledOff();
}

void loop()
{
  if(state == CODE || state == CHOOSEGATE)
  {
     keypad.getKey();
     if(keyTimeout != 0)
     {   
       if(millis() > keyTimeout)
       {
        resetState();
       }
       else
       {
        if(state == CODE)
        {
          ledCodeOn();
        }
        else if(state == CHOOSEGATE)
        {
          ledGateOn();
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
