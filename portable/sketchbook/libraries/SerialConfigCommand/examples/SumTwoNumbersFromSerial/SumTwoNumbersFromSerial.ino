// Example of how to use SerialConfigCommand to sum two numbers using the Serial Monitor.

/* - How to Use This Example -
 *  
 * After uploading the code, launch the Serial Monitor.
 * Line ending setting in Arduino Serial Monitor does not matter.
 * 
 * Type "num1=10" without quotes, press Enter.
 * "First number is 10" should echo back.
 * 
 * Type "num2=20" without quotes, press Enter.
 * "Second number is 20 should echo back"
 * 
 * Type "sum" without quotes, press Enter.
 * "10 + 20 = 30" should echo back.
 * 
 * Note: New Line and Carriage Return from the Serial Monitor will be filtered out so no need to fiddle with the Serial Monitor line ending settings.
 */

 /* - Types of Settings -
  *  
  * Users can enter an assigment or non-assigment setting.
  * Assignment setting: <command>=<value> (eg num1=10). 
  * Non-assignment setting: just <command> (eg sum), hasValue() returns false.
  */

#include <SerialConfigCommand.h>

int A=0, B=0, result;

SerialConfigCommand scc; //Define an instance of SerialConfigCommand

void setup() {
  
  Serial.begin(9600); // Begin your serial monitor first before setting up scc

  // Set the update speed to once every 200ms and callback function to be respond()
  // You can also use scc.set(respond) and it will default to update once every 300ms
  // Generally 100 to 300ms is a good range, the higher the number, the less responsive it feels but takes less processing power
  
  scc.set(200, respond); 
}



void loop() {
  scc.update(); // Remember to put update() in loop()
}


void respond(){ // Callback function
  
  // You must ensure user entered an assignment value with command, ie "num1=10" not "num1"
  // Thus check with scc.hasValue()  

  // You can compare using Arduino Strings
  if (scc.getCmdS()=="num1" && scc.hasValue()){ 
    A = scc.getValueInt();    
    Serial.println("First number is " + scc.getValueS());
  }

  // Or use C-string and strcmp function
  else if ( strcmp(scc.getCmd(), "num2")==0 && scc.hasValue() ){
    B = scc.getValueInt(); 
    Serial.print("Second number is ");
    Serial.println(scc.getValue());   
  }

  // In this case, need to check for non-assignment command with no value, ie "sum" not "sum=1"
  // Thus check with !scc.hasValue()  
  
  else if (scc.getCmdS()=="sum" && !scc.hasValue()){
    result = A+B;
    Serial.println(String(A) + " + " + String(B) + " = " + result);    
  }

  else {
    Serial.println("Invalid command received!");
  }
}
