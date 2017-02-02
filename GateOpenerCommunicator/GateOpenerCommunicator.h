#ifndef GateOpenerCommunicator_h
#define GateOpenerCommunicator_h

#include <RFM69.h>
#include <Arduino.h>

#define RECV true
#define SEND false
#define EMPTYMSG 0
#define VERIFYCODEMSG 1
#define TOKENMSG 2 
#define GATENUMMSG 3
#define OPENGATEMSG 4



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
		OpenGateMsg(byte* msg, int msgLen);
		int createOpenGateMsg(byte* buf);
		
};

class GateOpenerCommunicator
{
  public:
	byte RecvMessage[RF69_MAX_DATA_LEN];
    byte MessageLength;
    byte SenderId;  
	byte MessageToSend[RF69_MAX_DATA_LEN];
  
    void init(byte freqBand, byte myAddress, byte networkAddress, const char* encryptKey);
	
    boolean receive();
	byte getHeader();	
		
	boolean reply(TokenMsg msg);
	boolean reply(CodeMsg msg);
	boolean reply(GateNumMsg msg);
	boolean send(int senderId, TokenMsg msg);
	boolean send(int senderId, CodeMsg msg);
	boolean send(int senderId, GateNumMsg msg);
  private:  
	boolean sendMessage(int senderId, byte* buf, int messageLength);
    RFM69 _radio; 	
    void copyMessage();
};

class Logger
{
	public:
		void init();
		void log(GateNumMsg msg, byte counterpartId, boolean direction);
		void log(CodeMsg msg, byte counterpartId, boolean direction);
		void log(OpenGateMsg msg, byte counterpartId, boolean direction);
		void log(TokenMsg msg, byte counterpartId, boolean direction);		
		void log(UnknownMsg msg, byte counterpartId);
		void logDeliveryStatus(boolean ok);		
	private: 
		void logCounterpartId(byte counterpartId, boolean direction);
};

#endif
