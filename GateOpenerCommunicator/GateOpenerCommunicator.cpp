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
    RecvMessage[i] = _radio.DATA[i];
  }
  if (_radio.ACKRequested())
  {
    _radio.sendACK();
  }
}

boolean GateOpenerCommunicator::sendMessage(int senderId, byte* buf, int messageLength)
{ 
  if (_radio.sendWithRetry(senderId, buf, messageLength, 2, 20))
  {
    return true;     
  }
  else
  {
    return false;    
  }  
}

byte GateOpenerCommunicator::getHeader()
{
  if(MessageLength > 0)
  {
    return RecvMessage[0];  
  }
  return EMPTYMSG;
}

boolean GateOpenerCommunicator::reply(TokenMsg msg)
{
	return send(SenderId, msg);
}

boolean GateOpenerCommunicator::reply(CodeMsg msg)
{
	return send(SenderId, msg);
}

boolean GateOpenerCommunicator::send(int senderId, TokenMsg msg)
{
	int msgLen = msg.createTokenMsg(MessageToSend);
	return sendMessage(senderId, MessageToSend, msgLen);	
}

boolean GateOpenerCommunicator::send(int senderId, CodeMsg msg)
{
	int msgLen = msg.createCodeMsg(MessageToSend);
	return sendMessage(senderId, MessageToSend, msgLen);
}

TokenMsg::TokenMsg(boolean isValid, long token)
{
	IsValid = isValid;
	Token = token;
}

TokenMsg::TokenMsg(byte* msg, int msgLen)
{
	IsValid = msg[1] == 1;
	if(IsValid)
	{
		Token = ((long)msg[2]) + ((long)msg[3] << 8) + ((long)msg[4] << 16) + ((long)msg[5] << 24);
	}
	else
	{
		Token = -1;
	}	
}

int TokenMsg::createTokenMsg(byte* buf)
{
	buf[0]=TOKENMSG;
	if(IsValid==false)
	{
	  buf[1]=0;
	  return 2;  
	}
	else
	{
	  buf[1]=1;
	  longToByteArray(Token, buf, 2);
	  return 6;
	}
}

void TokenMsg::longToByteArray(long val, byte* buf, int startIndex)
{
	buf[startIndex]=(byte)val;
	buf[startIndex + 1]=(byte)(val >> 8);
	buf[startIndex + 2]=(byte)(val >> 16);
	buf[startIndex + 3]=(byte)(val >> 24);
}

CodeMsg::CodeMsg(char* code, int codeLength)
{
	Code = code;
	CodeLength = codeLength;
}

CodeMsg::CodeMsg(byte* msg, int msgLen)
{
	CodeLength = msgLen - 1;
	Code = (char*)(msg + 1);
}	

int CodeMsg::createCodeMsg(byte* buf)
{
	buf[0]=VERIFYCODEMSG;
	for(byte i=0;i<CodeLength;i++)
	{
		buf[i+1] = Code[i];  
	}
	return CodeLength+1;
}