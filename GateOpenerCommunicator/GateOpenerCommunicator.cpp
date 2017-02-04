#include "GateOpenerCommunicator.h"

void GateOpenerCommunicator::init(boolean mode, Random rnd, Logger logger)
{
	_connected = false;
	_mode = mode;
	_rnd = rnd;
	_logger = logger;
	char encryptKey[16];
	fillEncryptKey(encryptKey);
	_radio.initialize(FREQUENCY, _rnd.generateByteArd(0,254), NETWORKADDRESS);
	_radio.encrypt(encryptKey);
	
	connect();
}

void GateOpenerCommunicator::connect()
{
	if(_mode == SLAVE)
	{
		while(_connected == false)
		{
			long token = _rnd.generateLong();
			RequestAddressMsg msg = RequestAddressMsg(token);
			_logger.log(msg);
			broadcast(msg);
			
			while(true)
			{
				boolean received = receive(15000);
				if(received)
				{
					byte header = getHeader();		
					if(header == ADDRESSMSG)
					{
						AddressMsg addressMsg = AddressMsg(RecvMessage, MessageLength);
						if(addressMsg.Token == token)
						{
							_radio.initialize(FREQUENCY, addressMsg.Address, NETWORKADDRESS);
							_radio.encrypt(encryptKey);
							_connected = true;
							_masterAddress = SenderId;
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	else
	{	
		_connected = true;
	} 
}

void GateOpenerCommunicator::fillEncryptKey(char* buffer)
{
	byte isEncryptionKeyinEpprom = EEPROM.read(0);
	if(isEncryptionKeyinEpprom == 0)
	{
		for(byte i = 0;i < 16; i++)
		{
			buffer[i] = EEPROM.read(i+1);
		}	
	}
	else
	{
		for(byte i = 0; i < 16; i++)
		{
			buffer[i] = i;
		}	
		EEPROM.update(0, 0);
		for(byte i = 0;i < 16; i++)
		{
			EEPROM.update(i+1, buffer[i]);
		}
	}
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

boolean GateOpenerCommunicator::broadcast(RequestAddressMsg msg)
{
	int msgLen = msg.createRequestAddressMsg(MessageToSend);
	_radio.send(255, MessageToSend, msgLen, false);
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

boolean GateOpenerCommunicator::send(int receiverId, TokenMsg msg)
{
	int msgLen = msg.createTokenMsg(MessageToSend);
	return sendMessage(receiverId, MessageToSend, msgLen);	
}

boolean GateOpenerCommunicator::send(int receiverId, CodeMsg msg)
{
	int msgLen = msg.createCodeMsg(MessageToSend);
	return sendMessage(receiverId, MessageToSend, msgLen);
}

boolean GateOpenerCommunicator::send(int receiverId, GateNumMsg msg)
{
	int msgLen = msg.createGateNumMsg(MessageToSend);
	return sendMessage(receiverId, MessageToSend, msgLen);
}

boolean GateOpenerCommunicator::send(int receiverId, OpenGateMsg msg)
{
	int msgLen = msg.createOpenGateMsg(MessageToSend);
	return sendMessage(receiverId, MessageToSend, msgLen);
}

boolean GateOpenerCommunicator::send(int receiverId, AddressMsg msg)
{
	int msgLen = msg.createAddressMsg(MessageToSend);
	return sendMessage(receiverId, MessageToSend, msgLen);	
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

RequestAddressMsg::RequestAddressMsg(long token)
{
	Token = token;
}

int RequestAddressMsg::createRequestAddressMsg(byte* buf)
{
	buf[0]=REQUESTADDRESSMSG;
	Encoding::longToByteArray(Token, buf, 1);
	return 5;
}

RequestAddressMsg::RequestAddressMsg(byte* msg, int msgLen)
{
	Token = Encoding::byteArrayToLong(msg, 1);
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

AddressMsg::AddressMsg(long token, byte address)
{
	Token = token;
	Address = address;
}

AddressMsg::AddressMsg(byte* msg, int msgLen)
{
	Token = Encoding::byteArrayToLong(msg, 1);
	Address = msg[5];
}

int AddressMsg::AddressMsg(byte* buf)
{
	buf[0] = ADDRESSMSG;
	Encoding::longToByteArray(Token, buf, 1)
	buf[5] = Address;
	return 5;
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

void Logger::log(RequestAddressMsg msg, byte counterpartId, boolean direction)
{
	Serial.print("R ");
	Serial.print(msg.Token);
	logCounterpartId(counterpartId, direction);
	Serial.println();
}

void Logger::log(AddressMsg msg, byte counterpartId, boolean direction)
{
	Serial.print("A ");
	Serial.print(msg.Token);
	Serial.print(" ");
	Serial.print(msg.Address);
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

void Random::init(int analogPin, int pwmPin)
{
	_pwmPin = pwmPin;
	_analogPin = analogPin;
	pinMode(_analogPin, INPUT);
	pinMode(_pwmPin, OUTPUT);
	analogReference(EXTERNAL);
	analogWrite(_pwmPin, 127);
	_refNum = getNoise();
	randomSeed(generateLong());
}

byte Random::getNoise()
{
	int val = analogRead(_analogPin) - 510;
	return val < 0 ? (byte)(-val) : (byte)val;
}

byte Random::generateByte()
{
	byte randomNum = 0;
	byte pos = 0;
	
	while(pos < 8)
	{
		byte r = getNoise();
		for(byte i = 0; i < 4; i++)
		{
			byte twoBites = r & 0x03;
			if(twoBites == 1)
			{
				randomNum++;
				randomNum <<= 1;
				pos++;
			}
			else if(twoBites == 2)
			{
				randomNum <<= 1;
				pos++;
			}
			r >>= 2;
			if(pos == 8)
			{
				break;
			}
		}
	}
	return randomNum;
}

long Random::generateLong()
{
	return ((long)generateByte()) + ((long)generateByte() << 8) + ((long)generateByte() << 16) + ((long)generateByte() << 24);
}

int Random::generateInt()
{	
	return ({int}generateByte()) + ((int)generateByte() << 8);
}

byte Random::generateByteArd(byte min, byte max)
{
	return (byte)random(min, max);
}

int Random::generateIntArd(int min, int max)
{
	return random(min,max);
}