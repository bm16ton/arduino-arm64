#include "SerialConfigCommand.h"

SerialConfigCommand::SerialConfigCommand(){
// Empty Constructor
}



void SerialConfigCommand::set(void (*myFunction)()){
  // Assigns user input into privates
  active = true;
  timer = 0;
  period = 300; //Defaults to 300ms if period is not set
  callback_Function = myFunction; 
}



void SerialConfigCommand::set(unsigned long myPeriod,void (*myFunction)()){
  // Assigns user input into privates
  active = true;
	timer = 0;
	period = myPeriod;
	callback_Function = myFunction;	
}



void SerialConfigCommand::set(unsigned long myTimer,unsigned long myPeriod,void (*myFunction)()){
  // Assigns user input into privates
  active = true;
	period = myPeriod;
	timer = myTimer;  // Custom starting time
	callback_Function = myFunction;	
}



void SerialConfigCommand::reset(){
  timer = millis();
}



void SerialConfigCommand::disable(){
  active = false;
}



void SerialConfigCommand::enable(){
  active = true;
}



void SerialConfigCommand::setInterval(unsigned long myPeriod){
  period = myPeriod;
}



char* SerialConfigCommand::getCmd(){
  return cmd;  
}



String SerialConfigCommand::getCmdS(){
  return cmd;  
}



char* SerialConfigCommand::getValue(){
  return value;  
}



String SerialConfigCommand::getValueS(){
  return value;  
}



int SerialConfigCommand::getValueInt(){
  return atoi(value);
}



float SerialConfigCommand::getValueFloat(){
  return atof(value);
}



bool SerialConfigCommand::hasValue(){

  // if the first char is NULL, then it has no value
  if (value[0]=='\0'){  return false; }
  else{ return true; }  
}



void SerialConfigCommand::update(){

  // Make sure active and timer overflowed millis()
  if ( active && ((unsigned long)(millis()-timer) >= period) ) {

    // Read in the setting if Serial is available
    if (Serial.available() > 0){
      readSetting();
    }
    
	  timer = millis(); // Reset timer
    
  }
}



bool SerialConfigCommand::readSetting(){ // Function
// Read this flowchart for a better idea:
// https://drive.google.com/file/d/1MnBnaXLNSj7eUdXP4UXzL1iaxye_j8Mg/view?usp=sharing
// You will can open the file with draw.io or app.diagrams.net
// An image copy is also included in the source code

  // Only initialise if there is data in Serial buffer
  if (Serial.available()>0){
    cmd[0] = '\0';
    value[0] = '\0';
    readMode = READ_CMD;
  }

  while (Serial.available()>0){ // 1 While
    
    char myChar = Serial.read();

    if (Serial.available()<=0){ // 2 If

      if (myChar!='\n' && myChar!='\r'){ //3 If
        concatCmdValue(myChar); }  // 3 If
        
      callback_Function();
      return 1;  
       
    } // 2 If

    else { // 2 Else
      
      if (myChar=='\n' || myChar=='\r'){ // 3 If
        while ( Serial.available()>0 ){ Serial.read(); }
        callback_Function();
        return 1;
      } // 3 If
  
      else if (myChar=='='){ // 3 Else If
        readMode=READ_VALUE;
        continue;
      } // 3 Else If
  
      else{ // 3 Else
        if( concatCmdValue(myChar) ){ continue; }
        else{ return 0; }
      } // 3 Else
      
    } // 2 Else
  } // 1 While

  return 0;
}// Function



bool SerialConfigCommand::concatCmdValue(char myChar){ // Function
  if (readMode==READ_CMD){ // 1 If
    byte L = strlen(cmd);
    if (L<sizeof(cmd)-2){ // 2 If
      cmd[L] = myChar;
      cmd[L+1] = '\0';
    } // 2 If
    else{ return 0; }
  } // 1 If

  else{ // 1 Else
    byte L = strlen(value);
    if (L < sizeof(value)){ // 2 If
      value[L] = myChar;
      value[L+1] = '\0';
    } // 2 If
    else{  return 0; }
  } // 1 Else

  return 1;

} // Function
