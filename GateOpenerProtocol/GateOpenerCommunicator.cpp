#include "GateOpenerCommunicator.h"

GateOpenerCommunicator::GateOpenerCommunicator(byte freqBand, byte myAddress, byte networkAddress, const char* encryptKey)
{
  _radio.initialize(freqBand, myAddress,networkAddress);
  _radio.encrypt(encryptKey);
}

boolean GateOpenerCommunicator::receive()
{
  if (_radio.receiveDone())
  {
    copyMessage();    
    return true;
  }
  return false;
}

void GateOpenerCommunicator::copyMessage()
{
  SenderId = _radio.SENDERID;
  MessageLength = _radio.DATALEN;
  for (int i = 0; i < MessageLength; i++)
  {
    Message[i] = _radio.DATA[i];
  }
  if (_radio.ACKRequested())
  {
    _radio.sendACK();
  }
}

boolean GateOpenerCommunicator::sendMessage(byte receiverId, byte* buf, int messageLength)
{ 
  if (_radio.sendWithRetry(receiverId, buf, messageLength, 2, 20))
  {
    return true;     
  }
  else
  {
    return false;    
  }  
}
