#ifndef GateOpenerCommunicator_h
#define GateOpenerCommunicator_h

#include <RFM69.h>
#include <Arduino.h>   

class GateOpenerCommunicator
{
  public:
    GateOpenerCommunicator(byte freqBand, byte myAddress, byte networkAddress, const char* encryptKey);
    boolean receive();   
    boolean sendMessage(byte receiverId, byte* buf, int messageLength);
    
    byte Message[RF69_MAX_DATA_LEN];
    byte MessageLength;
    byte SenderId;
  private:
    RFM69 _radio;    
    void copyMessage();
};

#endif
