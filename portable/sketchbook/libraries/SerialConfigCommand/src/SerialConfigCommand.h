/*
SerialConfigCommand 0.4 by cygig
SerialConfigCommand is an easy way to adjust configurations via the Serial monitor. 
The library checks the Serial buffer every x interval defined by the user, and parse the input into commands and values.
Users can enter an assigment or non-assigment command.
Assignment command: <command>=<value> (eg num1=10). 
Non-assignment command: just <command> (eg sum), getValue returns 0, hasValue() returns false.
Checks by comparing Strings and Int can be done in the callback function using the parsed commands and values to dictate further actions.
*/

#ifndef SERIALCONFIGCOMMAND
#define SERIALCONFIGCOMMAND

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define READ_CMD 0
#define READ_VALUE 1
#define CMD_SIZE 32
#define VALUE_SIZE 16

class SerialConfigCommand {
  
  public:
    SerialConfigCommand();
    void set(void (*myFunction)());	
  	void set(unsigned long myPeriod,void (*myFunction)());
  	void set(unsigned long myTimer,unsigned long myPeriod,void (*myFunction)());
  	void reset();
  	void disable();
  	void enable();
  	void update();
	  void setInterval(unsigned long myPeriod);
   
    char* getCmd();
    String getCmdS();
    
    char* getValue();
    String getValueS();
    int getValueInt();
    float getValueFloat();
    bool hasValue();
    
    bool readSetting();
    

  private:
    void (*callback_Function)(); 
    bool active;
    unsigned long timer;
    unsigned long period;
    
    char cmd[CMD_SIZE];
    char value[VALUE_SIZE];
    
    bool concatCmdValue(char myChar);
    bool readMode=READ_CMD; 
    
    
};

#endif
