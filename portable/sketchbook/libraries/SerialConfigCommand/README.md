# SerialConfigCommand
SerialConfigCommand for Arduino allows user to set commands via the Serial Monitor easily. 

SerialConfigCommand can allow for commands with or without values to be set. Example: "LED=255", "Lock=1", "Start". It is compatible with both Arduino String and C-string, making it accessible for novice users. The library checks the Serial buffer every x interval defined by the user, and parse the setting into commands and values.

A setting consists of a command and value(which can be empty). Users can enter an assignment or non-assignment setting:
- Assignment setting: `command`=`value` (eg num1=10). 
- Non-assignment setting: just `command` (eg sum), `hasValue()` returns false.

User can then compare the command string or value string/integer/float in the callback function to dictate further actions.

New Line and Carriage Return from the Serial Monitor will be filtered out so no need to fiddle with the Serial Monitor line ending settings.

Only use common ANSI characters (those you see on a standard QWERTY keyboard) within the settings. **Do not use the equal sign '=' for commands or values**

---

# Update Notes
- v0.6
  - Corrected an error in callback function declaration. Declared it as returning void instead of nothing.
  
- v0.5
  - For the new version, `getCmd()` and `getValue()` will return C-string instead of Arduino String. To access Arduino String, `use getCmdS()` or `getValueS()`.

---

# Installation
Download the codes, unzip it, copy `SerialConfigCommand` folder into your Arduino library folder, for example `C:\Users\you\Documents\Arduino\libraries`.
You can also get it via the Arduino Library Manager.

---

# Default Values

The default values for some of the C-strings are:
- 32 bytes for command
- 16 bytes for value

One byte stores one character, and one NULL character is present at the end of the string. As such, never have a command that is over 31 character in length or a value that is over 15 characters in length. 

Feel free to change those in the header file, but take note that most Arduinos only have a Serial buffer of 64 bytes in total. The total should never exceed 256 since bytes are use to store string lengths for operation.

---

# Public Functions

## SerialConfigCommand()
Empty constructor.

## _void_ set(void (*myFunction)())
Setup the instance by defining the name of the callback function. The period is by default set to 300ms (ie the instance check for updates in the Serial Monitor every 300ms.

## _void_ set(unsigned long myPeriod,void (*myFunction)())
Setup the instant by definining the period (how often the instance checks for updates from the Serial Monitor) and the name of the callback function.

## _void_ set(unsigned long myTimer,unsigned long myPeriod,void (*myFunction)())
Similar to the above two, but you get to set the starting time used to compared to how many milliseconds passed while running the program.

## _void_ reset()
Resets the timer.

## _void_ disable()
Disables core functionalities of this instance of SerialConfigCommand.

## _void_ enable()
Enables core functionalities of this instance of SerialConfigCommand.

## _void_ update()
This function has to be put in `loop()`.

## _void_ setInterval(unsigned long myPeriod)
Set the period.

## _char*_ getCmd()
Returns a C-string of the current command.

## _String_ getCmdS()
Returns a Arduino String of the current command.

## _char*_ getValue()
Returns a C-string of the value of the current command.

## _String_ getValue()
Returns a Arduino String of the value of the current command.

## _int_ getValueInt()
Returns the value of the current command as an integer.

## _int_ getValueInt()
Returns the value of the current command as a floating point number.

## _bool_ hasValue()
Returns true if the current command has a value.

## _bool_ readSetting()
This calls the function the read and parse the data in the Serial buffer once. This is usually not used as the library will keep checking for data in the Serial buffer, but could be useful for Arduinos which can use the serialEvent() function. This function is thus set to public. If used with serialEvent(), there is no need to use update() in loop(). Returns 1 if settings read successfully, other 0 for errors or if there is nothing in the Serial buffer.

---

# Flowcharts
In lieu of commands, the flowcharts below will explain the process of reading and parsing settings from the Serial buffer.

![image](flowchart/SerialConfigCommand%20Read%20Setting_Page_1.jpg)
![image](flowchart/SerialConfigCommand%20Read%20Setting_Page_2.jpg)

---