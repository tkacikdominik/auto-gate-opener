/******P I N S********/
const byte relay1 = 11;
const byte relay2 = 12;

/*******C O D E******/ 
const byte maxCodeLength = 8;
const char emptySymbol = 'x';
          
char readCode[maxCodeLength];
char code[maxCodeLength] = {'0','9','0','5','1','9','9','7'};

/**V A R I A B L E**/
boolean check = true;

void setup() 
{
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  resetCode();
}

void loop() 
{
  if(verifyCode())
  {
    digitalWrite(relay2, HIGH);
    delay(2000);
    digitalWrite(relay2, LOW);
  }
}

void resetCode()
{
  for(byte i=0;i<maxCodeLength;i++)
  {
    readCode[i]=0;  
  }
}

boolean verifyCode()
{
  for(byte i=0;i<maxCodeLength;i++)
  {
    if(readCode[i] != code[i])
    {
      return false;   
      resetCode();
    }
  }
  return check;
  resetCode();
}
