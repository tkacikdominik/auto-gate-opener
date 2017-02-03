#include "GateOpenerCommunicator.h"

void GateOpenerCommunicator::init(byte freqBand, byte myAddress, byte networkAddress, const char* encryptKey)
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

boolean GateOpenerCommunicator::receive(unsigned long timeoutMillis)
{
	unsigned long maxTime = millis()+timeoutMillis;
	while(1)
	{
		boolean received = receive();
		if(received)
		{
			return true;
		}
		if(millis() > maxTime)
		{
			break;
		}
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

boolean GateOpenerCommunicator::reply(GateNumMsg msg)
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

boolean GateOpenerCommunicator::send(int senderId, GateNumMsg msg)
{
	int msgLen = msg.createGateNumMsg(MessageToSend);
	return sendMessage(senderId, MessageToSend, msgLen);
}

boolean GateOpenerCommunicator::send(int senderId, OpenGateMsg msg)
{
	int msgLen = msg.createOpenGateMsg(MessageToSend);
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
		Token = Encoding::byteArrayToLong(msg, 2);
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
	  Encoding::longToByteArray(Token, buf, 2);
	  return 6;
	}
}

void Encoding::longToByteArray(long val, byte* buf, int startIndex)
{
	buf[startIndex]=(byte)val;
	buf[startIndex + 1]=(byte)(val >> 8);
	buf[startIndex + 2]=(byte)(val >> 16);
	buf[startIndex + 3]=(byte)(val >> 24);
}

long Encoding::byteArrayToLong(byte* buf, int startIndex)
{
	return ((long)buf[startIndex]) + ((long)buf[startIndex+1] << 8) + ((long)buf[startIndex+2] << 16) + ((long)buf[startIndex+3] << 24);
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

GateNumMsg::GateNumMsg(long token, byte gateId)
{
	Token = token;
	GateId = gateId;
}

GateNumMsg::GateNumMsg(byte* msg, int msgLen)
{
	Token = Encoding::byteArrayToLong(msg,1);
	GateId = msg[5];
}

int GateNumMsg::createGateNumMsg(byte* buf)
{
	buf[0]= GATENUMMSG;
	Encoding::longToByteArray(Token, buf, 1);
	buf[5]= GateId;
	return 6;
}

int OpenGateMsg::createOpenGateMsg(byte* buf)
{
	buf[0] = OPENGATEMSG;
	return 1;
}

OpenGateMsg::OpenGateMsg(byte* msg, int msgLen)
{
	
}

OpenGateMsg::OpenGateMsg()
{
	
}

void Logger::init()
{
	Serial.begin(9600);
}

void Logger::log(GateNumMsg msg,  byte counterpartId, boolean direction)
{
	 Serial.print("G ");
	 Serial.print(msg.Token);
	 Serial.print("  ");
	 Serial.print(msg.GateId);
	 logCounterpartId(counterpartId, direction);
	 Serial.println();
}

void Logger::log(CodeMsg msg, byte counterpartId, boolean direction)
{
	Serial.print("C ");
	for(byte i = 0; i < msg.CodeLength; i++)
	{
		Serial.print((char)msg.Code[i]);
	}
	Serial.print(" ");
	Serial.print(msg.CodeLength);
	logCounterpartId(counterpartId, direction);
	Serial.println();
}

void Logger::log(TokenMsg msg, byte counterpartId, boolean direction)
{
	Serial.print("T ");
	Serial.print(msg.IsValid);
	Serial.print(" ");
	Serial.print(msg.Token);
	logCounterpartId(counterpartId, direction);
	Serial.println();
}

void Logger::logDeliveryStatus(boolean ok)
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

void Logger::logCounterpartId(byte counterpartId, boolean direction)
{
	Serial.print("[");
	if(direction==SEND)
	{
		Serial.print("->");
	}
	Serial.print(counterpartId);
	if(direction==RECV)
	{
		Serial.print("->");
	}
	Serial.print("]");
	
}

void Logger::log(UnknownMsg msg, byte counterpartId)
{
	Serial.print("U ");
	for(byte i = 0; i < msg.MessageLength; i++)
	{
		Serial.print(msg.Message[i], HEX); 
		Serial.print(" ");
	}
	logCounterpartId(counterpartId, RECV);
	Serial.println();
}

UnknownMsg::UnknownMsg(byte* msg, int msgLen)
{
	Message = msg;
	MessageLength = msgLen;
}

void Logger::log(OpenGateMsg msg, byte counterpartId, boolean direction)
{
	Serial.print("O ");
	logCounterpartId(counterpartId, direction);
	Serial.println();
}