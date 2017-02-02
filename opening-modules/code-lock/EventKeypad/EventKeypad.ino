/* ack nefunguje, co ak neodpoveda, 
*/
#include <GateOpenerCommunicator.h>
#include <Keypad.h>

#define MASTERADDRESS 1
#define CODELOCKADDRESS 2
#define NETWORKADDRESS 0
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "TOPSECRETPASSWRD" 
#define USEACK        true // Request ACKs or not

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

byte rowsPin[rows] = {7, 8, 9, A0};
byte colsPin[cols] = {3, 4, 5, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowsPin, colsPin, rows, cols); 

GateOpenerCommunicator communicator = GateOpenerCommunicator(FREQUENCY, CODELOCKADDRESS, NETWORKADDRESS, ENCRYPTKEY);
Logger logger = Logger();

// code
const byte maxCodeLength = 8;
char readCode[maxCodeLength];
byte pos;

// piezo pin 
byte piezo = A1;

byte state;
long actualToken;

void setup(){
    keypad.addEventListener(keypadEvent);
    resetCodePos();
    pinMode(piezo, OUTPUT);
    state = CODE;
}

void loop()
{
  if(state == CODE || state == CHOOSEGATE)
  {
     keypad.getKey(); 
  }
  else if (state == COMMUNICATION)
  {
    if (communicator.receive())
    {
      processMessage();
    }
  }
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
    }
  }
}

void processKeyChooseGate(KeypadEvent key)
{
  GateNumMsg gateNumMsg = GateNumMsg(actualToken, 1);  
  logger.log(gateNumMsg, MASTERADDRESS, SEND);
  boolean ok = communicator.send(MASTERADDRESS, gateNumMsg);
  logger.logDeliveryStatus(ok);
}

void processKeyCode(KeypadEvent key)
{
 if(key=='#')
  {
    doublePip();
    sendCodeToMaster();  
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
  logger.log(msg, communicator.SenderId, SEND);

  if(msg.IsValid)
  {
    actualToken = msg.Token;
    state = CHOOSEGATE; 
  }
  else
  {
    state = CODE;
    invalidPip();
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
  logger.log(msg, communicator.SenderId, SEND);
  
  boolean ok = communicator.send(MASTERADDRESS, msg);
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

void invalidPip()
{
  tone(piezo, 1200, 200);
  delay(250);
  tone(piezo, 800, 200);
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
