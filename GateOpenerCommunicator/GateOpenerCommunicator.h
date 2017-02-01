#ifndef GateOpenerCommunicator_h
#define GateOpenerCommunicator_h

#include <RFM69.h>
#include <Arduino.h>

#define EMPTYMSG 0
#define VERIFYCODEMSG 1
#define TOKENMSG 2   

class TokenMsg
{
	public:
		TokenMsg(boolean isValid, long token);
		TokenMsg(byte* msg, int msgLen);
		boolean IsValid;
		long Token;
		int createTokenMsg(byte* buf);
	private:
		void longToByteArray(long val, byte* buf, int startIndex);   
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

class GateOpenerCommunicator
{
  public:
	byte RecvMessage[RF69_MAX_DATA_LEN];
    byte MessageLength;
    byte SenderId;  
	byte MessageToSend[RF69_MAX_DATA_LEN];
  
    GateOpenerCommunicator(byte freqBand, byte myAddress, byte networkAddress, const char* encryptKey);
	
    boolean receive();
	byte getHeader();	
		
	boolean reply(TokenMsg msg);
	boolean reply(CodeMsg msg);
	boolean send(int senderId, TokenMsg msg);
	boolean send(int senderId, CodeMsg);
  private:  
	boolean sendMessage(int senderId, byte* buf, int messageLength);
    RFM69 _radio; 	
    void copyMessage();
};

#endif
