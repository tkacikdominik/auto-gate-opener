#ifndef GateOpenerCommunicator_h
#define GateOpenerCommunicator_h

#include <RFM69.h>
#include <Arduino.h>
#include <EEPROM.h>

#define RECV true
#define SEND false

#define MASTER true
#define SLAVE false

#define EMPTYMSG 0
#define VERIFYCODEMSG 1
#define TOKENMSG 2 
#define GATENUMMSG 3
#define OPENGATEMSG 4
#define REQUESTADDRESSMSG 5
#define ADDRESSMSG 6 

#define NETWORKADDRESS 0
#define FREQUENCY RF69_868MHZ


class Encoding
{
	public:
		static void longToByteArray(long val, byte* buf, int startIndex); 
		static long byteArrayToLong(byte* buf, int startIndex);
};

class TokenMsg
{
	public:
		TokenMsg(boolean isValid, long token);
		TokenMsg(byte* msg, int msgLen);
		
		boolean IsValid;
		long Token;
		
		int createTokenMsg(byte* buf); 
};

class CodeMsg
{
	public:
		CodeMsg(char* code, int codeLength);
		CodeMsg(byte* msg, int msgLen);		
		
		char* Code;
		int CodeLength;
		
		int createCodeMsg(byte* buf);
};

class GateNumMsg
{
	public:
		GateNumMsg(long token, byte gateId);
		GateNumMsg(byte* msg, int msgLen);
		
		long Token;
		byte GateId;
		
		int createGateNumMsg(byte* buf);
};

class UnknownMsg
{
	public:
		UnknownMsg(byte* msg, int msgLen);
	
		int MessageLength;
		byte* Message;
};

class OpenGateMsg
{
	public:
		OpenGateMsg();
		OpenGateMsg(byte* msg, int msgLen);
		int createOpenGateMsg(byte* buf);
		
};

class RequestAddressMsg
{
	public:
		RequestAddressMsg(long token);
		RequestAddressMsg(byte* msg, int msgLen);
		
		long Token;
		
		int createRequestAddressMsg(byte* buf); 
};

class AddressMsg
{
	public:
		AddressMsg(long token, byte address);
		AddressMsg(byte* msg, int msgLen);
		
		byte Address;
		long Token;
		
		int createAddressMsg(byte* buf); 
};

class Random
{
	public:
		void init(int analogPin, int pwmPin);
		byte generateByte();
		long generateLong();
		int generateInt();
		byte generateByteArd(byte min, byte max);
		int generateIntArd(int min, int max);
		byte getNoise();
	private:
		int _refNum;
		int _analogPin;
		int _pwmPin;
};

class Logger
{
	public:
		void init();
		void log(GateNumMsg msg, byte counterpartId, boolean direction);
		void log(CodeMsg msg, byte counterpartId, boolean direction);
		void log(OpenGateMsg msg, byte counterpartId, boolean direction);
		void log(TokenMsg msg, byte counterpartId, boolean direction);		
		void log(RequestAddressMsg msg, byte counterpartId, boolean direction);
		void log(AddressMsg msg, byte counterpartId, boolean direction);
		void log(UnknownMsg msg, byte counterpartId);
		void logDeliveryStatus(boolean ok);		
	private: 
		void logCounterpartId(byte counterpartId, boolean direction);
};

class GateOpenerCommunicator
{
  public:
	byte RecvMessage[RF69_MAX_DATA_LEN];
    byte MessageLength;
    byte SenderId;  
	byte MessageToSend[RF69_MAX_DATA_LEN];
	byte MyAddress;
	byte MasterAddress;
    
	void init(boolean mode, Random rnd, Logger logger);
	
    boolean receive(unsigned long timeoutMillis);
	boolean receive();
	byte getHeader();	
		
	boolean reply(TokenMsg msg);
	boolean reply(CodeMsg msg);
	boolean reply(GateNumMsg msg);
	boolean reply(AddressMsg msg);
	boolean send(int receiverId, TokenMsg msg);
	boolean send(int receiverId, CodeMsg msg);
	boolean send(int receiverId, OpenGateMsg msg);
	boolean send(int receiverId, GateNumMsg msg);
	boolean send(int receiverId, AddressMsg msg);
	boolean broadcast(RequestAddressMsg msg);
  private:  	
	
	boolean _mode;
	boolean _connected;
	RFM69 _radio; 
	Random _rnd;
	Logger _logger;
	char _encryptKey[16];
	
	boolean sendMessage(int senderId, byte* buf, int messageLength);    	
    void copyMessage();
	void fillEncryptKey();
	void connect();
};





#endif
