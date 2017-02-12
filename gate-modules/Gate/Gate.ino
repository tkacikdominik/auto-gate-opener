#include <GateOpenerCommunicator.h>

Logger logger;
GateOpenerCommunicator communicator;
Random rnd;

byte gateOutput = 3;
const byte dip[] = {A0 ,A1 , A2, A3};
const byte analogPin = A5;
const byte pwmPin = 9;
byte myDipAddress;



void setup() 
{
  pinMode(3, OUTPUT);
  for(byte i = 0;i < 4;i++)
  {
    pinMode(dip[i], INPUT);
    digitalWrite(dip[i], HIGH);
  }
  myDipAddress = getDipAddress();
  rnd.init(analogPin, pwmPin);
  logger.init();
  communicator.init(SLAVE, rnd, logger);
  GateIdMsg gateIdMsg = GateIdMsg(myDipAddress);
  logger.log(gateIdMsg, communicator.MasterAddress, SEND);
  communicator.send(communicator.MasterAddress, gateIdMsg);
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
  digitalWrite(gateOutput, HIGH);
  delay(2000);
  digitalWrite(gateOutput, LOW); 
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
