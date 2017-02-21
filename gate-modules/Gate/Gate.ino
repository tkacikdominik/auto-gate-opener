#include <GateOpenerCommunicator.h>

Logger logger;
GateOpenerCommunicator communicator;
Random rnd;

byte gateRelay = 7;
byte lightRelay = 8;
const byte dip[] = {A0 ,A1 , A2, A3, A4};
const byte analogPin = A5;
const byte pwmPin = 9;
byte myDipAddress;



void setup() 
{
  pinMode(gateRelay, OUTPUT);
  pinMode(lightRelay, OUTPUT);
  for(byte i = 0;i < 5;i++)
  {
    pinMode(dip[i], INPUT);
    digitalWrite(dip[i], HIGH);
  }
  myDipAddress = getDipAddress();
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger, afterConnect); 
}

void afterConnect()
{
  GateIdMsg gateIdMsg = GateIdMsg(myDipAddress);
  logger.log(gateIdMsg, communicator.MasterAddress, SEND);
  communicator.send(communicator.MasterAddress, gateIdMsg);
}

void loop() 
{
  communicator.refresh();
  if (communicator.receive())
  {
    processMessage();
  }  
}

void processMessage()
{
  switch(communicator.getHeader())
  {
    case OPENGATEMSG:
    {
      openGateMsgHandler();
      break;
    }
    default:
    {
      unknownMsgHandler();
      break;
    }  
  }  
}

void openGateMsgHandler()
{
  OpenGateMsg openGateMsg = OpenGateMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(openGateMsg, communicator.SenderId, RECV);
  openGate();
}

void openGate()
{
    if(getLightstatus())
  {
    digitalWrite(lightRelay, HIGH);
    digitalWrite(gateRelay, HIGH);
    delay(2000);
    digitalWrite(gateRelay, LOW); 
    delay(8000);
    digitalWrite(lightRelay, LOW);   
  }
  else 
  {
    digitalWrite(gateRelay, HIGH);
    delay(2000);
    digitalWrite(gateRelay, LOW); 
  }
}

void unknownMsgHandler()
{
  UnknownMsg unknownMsg = UnknownMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(unknownMsg, communicator.SenderId);
}

byte getDipAddress()
{
  int dipAddress = 0;
  for(byte i = 0;i < 4; i++)
  {
    dipAddress = (dipAddress << 1) | digitalRead(dip[i]);  
  }
  return dipAddress; 
}

boolean getLightstatus()
{
  byte val = digitalRead(dip[4]);
  if(val==1)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

