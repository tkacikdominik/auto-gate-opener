#include <GateOpenerCommunicator.h>

  #define MASTERADDRESS 1
  #define GATEADDRESS 3 

Logger logger;
GateOpenerCommunicator communicator;

byte gateLed = 3;

void setup() 
{
  pinMode(3, OUTPUT);
  logger.init();
  communicator.init(GATEADDRESS);
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
  digitalWrite(gateLed, HIGH);
  delay(2000);
  digitalWrite(gateLed, LOW); 
}

void unknownMsgHandler()
{
  UnknownMsg unknownMsg = UnknownMsg(communicator.RecvMessage, communicator.MessageLength);
  logger.log(unknownMsg, communicator.SenderId);
}
