  
/*
    Name:       Teensy_I2C_Sniffer_V11.ino
    Created:  1/18/2020 10:55:55 AM
    Author:     FRANKNEWXPS15\Frank
*/
/* 'Notes:

    A typical I2C sentence when communicating with a MPU6050 IMU module goes something like:
        "I2C(68) wrote 1 byte to 75 - C0 Done."
        "I2C(68) wrote 3 bytes to 72 - C0 0C 10 Done."
        "I2C(68) read 5 bytes from 6A - C0 0C 10 14 03 Done."

    To form a sentence, we need:
        Device addr: 68 in the above examples
        Read/Write direction
        To/From register address:  75, 72 and 6A in the above examples
        Data:  C0, C0 0C 10, and C0 0C 10 14 03 in the above examples
        number of bytes written/read:  1,3 & 5 in the above examples

     Each I2C communication proceeds as follows (assuming a START from an IDLE condition):
         A START or RESTART condition, denoted by SDA & SCL HIGH, followed by SDA LOW, SCL HIGH
         A 7-bit device address, MSB first (0x8/0xC = 1, 0x0/0x4 = 0)
         A R/W bit (0x8/0xC = read, 0x0/0x4 = write)
         An ACK bit (0x8/0xC = NAK, 0x0/0x4 = ACK)
         If the bus direction is WRITE, then
             A register address for read/write
             zero or more additional data bytes
         Else (the bus direction is READ)
            One or more additional data bytes
         Endif

    This version uses a fixed-size (2048 bytes) array instead of tonton81's circular buffer library.

    To generalize for any I2C slave device rather than just the MPU6050 IMU, comment out the
    "#define MPU6050_SPECIFIC line below. This will remove all MPU6050 specific code
*/



#include <avr/pgmspace.h>

#define outputStream Serial3

#define Serial3begin(...)    outputStream.begin(__VA_ARGS__)
#define Serial3print(...)    outputStream.print(__VA_ARGS__)
#define Serial3write(...)    outputStream.print(__VA_ARGS__)
#define Serial3println(...)  outputStream.println(__VA_ARGS__)

#include <TimerOne.h> //needed for ISR
#include <TeensyThreads.h>

#if   defined(TEENSY_40)    // Teensy v4 usable digital are: A0-A9; A0-A9 are always digital 14-23, for Arduino compatibility
 byte       pins[] = {  A0 ,  A1 ,  A2 ,  A3 ,  A4 ,  A5 ,  A6 ,  A7, A8, A9  };
 String pinnames[] = { "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9" };
#elif   defined(KINETISK)   // Teensy v3 usable digital are: A0-A7. 13=LED
 byte       pins[] = {  A0 ,  A1 ,  A2 ,  A3 ,  A4 ,  A5 ,  A6 ,  A7  };
 String pinnames[] = { "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7" };
#elif defined(CORE_TEENSY)  // Teensy v2
 byte       pins[] = { PINB2 , PINB3 , PINB6 , PINB4 , PINB1  };
 String pinnames[] = {    "B2",    "B3",    "B6",    "B4",    "B1" };
#elif defined(ENERGIA)     // TI Launchpad Tiva C
 byte       pins[] = {  PA_5,   PB_4,   PE_5,   PE_4,   PB_1  };
 String pinnames[] = { "PA_5", "PB_4", "PE_5", "PE_4", "PB_1" };
#elif defined(STM32)       // STM32 bluepill, pinout is here: https://wiki.stm32duino.com/index.php?title=File:Bluepillpinout.gif. See also instructions to get it running with the Arduino IDE here: http://www.zoobab.com/bluepill-arduinoide
 byte       pins[] = {  10 ,  11 ,  12 ,  13 ,  14 ,  15 ,  16 ,  17, 18 , 19 , 21 , 22  };
 String pinnames[] = { "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "21", "22" };
#elif defined(ESP_H)       // ESP8266 Wemos D1 Mini. if properly not set may trigger watchdog
 byte       pins[] = {  D1 ,  D2 ,  D3 ,  D4 ,  D5 ,  D6 ,  D7 ,  D8  };
 String pinnames[] = { "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8" };
#else                      // DEFAULT
                           // Arduino Pro. usable digital 2-12,14-10. 13=LED 0,1=serial
 byte       pins[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
 String pinnames[] = { "DIG_2", "DIG_3", "DIG_4", "DIG_5" , "DIG_6",
                       "DIG_7", "DIG_8", "DIG_9", "DIG_10", "DIG_11"};
  #include <EEPROM.h>
  #define EEPROMSTORE
#endif

#define PIN_NOT_SET 0xff
String jtagpinnames[] = { "TCK", "TMS", "TDO", "TDI", "TRST" };

byte TCK  = PIN_NOT_SET;
byte TMS  = PIN_NOT_SET;
byte TDO  = PIN_NOT_SET;
byte TDI  = PIN_NOT_SET;
byte TRST = PIN_NOT_SET;

// Pattern used for scan() and loopback() tests
#define PATTERN_LEN              64
// Use something random when trying find JTAG lines:
static char pattern[PATTERN_LEN] = "0110011101001101101000010111001001";
// Use something more determinate when trying to find
// length of the DR register:
//static char pattern[PATTERN_LEN] = "1000000000000000000000000000000000";

// Max. number of JTAG enabled chips (MAX_DEV_NR) and length
// of the DR register together define the number of
// iterations to run for scan_idcode():
#define MAX_DEV_NR               8
#define IDCODE_LEN               32

// Target specific, check your documentation or guess
#define SCAN_LEN                 1890 // used for IR enum. bigger the better
#define IR_LEN                   5
// IR registers must be IR_LEN wide:
#define IR_IDCODE                "01100" // always 011
#define IR_SAMPLE                "10100" // always 101
#define IR_PRELOAD               IR_SAMPLE

/*
 * END USER DEFINITIONS
 */



// TAP TMS states we care to use. NOTE: MSB sent first
// Meaning ALL TAP and IR codes have their leftmost
// bit sent first. This might be the reverse of what
// documentation for your target(s) show.
#define TAP_RESET                "11111"       // looping 1 will return
                                               // IDCODE if reg available
#define TAP_SHIFTDR              "111110100"
#define TAP_SHIFTIR              "1111101100" // -11111> Reset -0> Idle -1> SelectDR
                                              // -1> SelectIR -0> CaptureIR -0> ShiftIR

// Ignore TCK, TMS use in loopback check:
#define IGNOREPIN                0xFFFF
// Flags configured by UI:
#define TRUE                     255
#define FALSE                    0
boolean VERBOSE                  = FALSE;
boolean DELAY                    = FALSE;
long    DELAYUS                  = 50;
boolean PULLUP                   = TRUE;

const byte pinslen               = sizeof(pins)/sizeof(pins[0]);

// For 3.3v AVR boards. Cuts clock in half. Also see cmd in setup()
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))


/*
* Return a pointer to the jtag pins given an index.
*
*/
byte * jtag_ptr_from_idx(const int idx){
  byte * curr;
  switch (idx) {
      case 0:
        curr = &TCK;
        break;
      case 1:
        curr = &TMS;
        break;
      case 2:
        curr = &TDO;
        break;
      case 3:
        curr = &TDI;
        break;
      case 4:
        curr = &TRST;
        break;
  }
  return curr;
}

//#define PARSE_LOOP_DEBUG

const uint16_t CAPTURE_ARRAY_SIZE = 2048;
const uint16_t VALID_DATA_ARRAY_SIZE = 2048;
const int WAITING_PRINT_INTERVAL_MSEC = 200;//interval timer for 'Waiting for data...' printout

#define MONITOR_OUT1 2 //so can monitor ISR activity with O'scope
#define MONITOR_OUT2 3 //so can monitor ISR activity with O'scope
#define MONITOR_OUT3 4 //so can monitor ISR activity with O'scope
#define SDA_PIN 18
#define SCL_PIN 19

#pragma region PROCESSING_VARIABLES
uint8_t devAddr;
uint8_t regAddr;
uint8_t databytes[2048]; //holds multiple databytes for later output sentence construction
uint16_t numbytes = 0; //number of data bytes extracted from data stream
int ACKNAKFlag; //can be negative
uint16_t databyte_idx = 0; //index into databyte_array
uint8_t killbuff[2]; //used to consume start/stop bytes
elapsedMillis mSecSinceLastWaitingPrint;
uint8_t valid_data[2048];
uint16_t numvalidbytes = 0; //number of valid bytes in this burst
uint16_t read_idx = 0; //pointer to next byte pair to be processed

//added for bus direction labels
enum BUSDIR
{
  WRITE,
  READ,
  UNKNOWN = -1
} RWDir;
BUSDIR BusDir = BUSDIR::UNKNOWN;
#pragma endregion ProcVars


#pragma region ISR_SUPPORT
uint8_t raw_data[CAPTURE_ARRAY_SIZE]; //holds data captured from I2C bus
volatile uint16_t  write_idx = 0;
volatile uint8_t   current_portb = 0xFF;
volatile uint8_t   last_portb = 0xFF;
volatile uint16_t mult0xCCount = 0;
const uint16_t MAX_IDLE_COUNT = 2500;

volatile bool bDone = false;
volatile bool bWaitingForStart = true;
volatile bool bIsData = true;
volatile bool bIsStart = false;
volatile bool bIsStop = false;
volatile uint8_t last_current;
#pragma endregion ISR Support

void setup()
{
//  pinMode(14, INPUT);
//  buttonState = digitalRead(14);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial1.begin(9600);
  SerialUSB2.begin(9600);
  Serial.begin(460800);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
threads.addThread(serpass, 1);
delay(100);
    Serial.println("Hack The Dumpster");
    Serial.println("Type a for i2c scanner");
    Serial.println("Type b for jtagenum"); 
}

void loop()
{
    while (Serial.available() > 0) {
    char inByte = Serial.read();
    switch (inByte) {
      case 'a':
      i2cscansetup();
      delay(100);
      i2cscan(); 
      break;
      
      case 'b':
      Serial.println("Welcome to jtagenum. press h for help");
      setupjtagenum();
      delay(100);
      while(1)
       jtagenum(); 
    }
      }
}

void serpass() {
  while(1)
  if (SerialUSB2.available()) {      // If anything comes in Serial (USB),
    Serial1.write(SerialUSB2.read());   // read it and send it out Serial1 (pins 0 & 1)
  }  else if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    SerialUSB2.write(Serial1.read());   // read it and send it out Serial (USB)
  }
}
void i2cscansetup()
{
  unsigned long now = millis();
  pinMode(5, OUTPUT);
  int idx = 0;
  while (!Serial && (millis() - now) < 3000)
  {
    delay(500);
    idx++;
  }
  Serial.printf("Serial available after %lu mSec\n", millis() - now);
  pinMode(MONITOR_OUT1, OUTPUT);
  digitalWrite(MONITOR_OUT1, LOW);
  pinMode(MONITOR_OUT2, OUTPUT);
  digitalWrite(MONITOR_OUT2, LOW);
  pinMode(MONITOR_OUT3, OUTPUT);
  digitalWrite(MONITOR_OUT3, LOW);

  Serial3begin(2000000);
  Serial3println("HELLO");
  Serial3println(F("F HELLO"));
  Serial3println(123.456, 2);
  Serial3println(3735928559UL, HEX);
  pinMode(SCL_PIN, INPUT);
  pinMode(SDA_PIN, INPUT);

  //reset port byte vars & start timer
  last_portb = current_portb = 0;
  write_idx = 0;
  memset(raw_data, 255, CAPTURE_ARRAY_SIZE);
  //PrintNextArrayBytes(raw_data, 255, 20);
//  Timer1.initialize(1); // run every mico second
#if 1
  Timer1_initialize(0.5); // run every half micro second
#else
  Timer1.initialize(1); // run every half micro second
#endif

  Timer1.attachInterrupt(capture_data);


  mSecSinceLastWaitingPrint = 0;
}
#if 1
void Timer1_initialize(float float_ms) {
  uint32_t period = (float)F_BUS_ACTUAL * float_ms * 0.0000005f;
  uint32_t prescale = 0;
  while (period > 32767) {
    period = period >> 1;
    if (++prescale > 7) {
      prescale = 7; // when F_BUS is 150 MHz, longest
      period = 32767; // period is 55922 us (~17.9 Hz)
      break;
    }
  }
  //Serial.printf("setPeriod, period=%u, prescale=%u\n", period, prescale);
  FLEXPWM1_FCTRL0 |= FLEXPWM_FCTRL0_FLVL(8); // logic high = fault
  FLEXPWM1_FSTS0 = 0x0008; // clear fault status
  FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(8);
  FLEXPWM1_SM3CTRL2 = FLEXPWM_SMCTRL2_INDEP;
  FLEXPWM1_SM3CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale);
  FLEXPWM1_SM3INIT = -period;
  FLEXPWM1_SM3VAL0 = 0;
  FLEXPWM1_SM3VAL1 = period;
  FLEXPWM1_SM3VAL2 = 0;
  FLEXPWM1_SM3VAL3 = 0;
  FLEXPWM1_SM3VAL4 = 0;
  FLEXPWM1_SM3VAL5 = 0;
  FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(8) | FLEXPWM_MCTRL_RUN(8);
 // pwmPeriod = period;
}

#endif
//............................... jtagnum setup .............................
//............................................................................
void setupjtagenum()
{
#ifdef HALFCLOCK
        // for 3.3v boards. Cuts clock in half
        // normally only on avr based arduino & teensy hardware
        CPU_PRESCALE(0x01);
#endif
 //       Serial.begin(115200);
 // Serial.print("jtag setup");
        byte *curr;
        for (int i = 0; i < 5; ++i) {
          curr = jtag_ptr_from_idx(i);
          if (*curr == PIN_NOT_SET) {
#ifdef EEPROMSTORE
            // if we are on arduino we can save/restore jtag pins from
            // the eeprom
            *curr = EEPROM.read(i);
#endif
            if( *curr < 0 || *curr >= pinslen){
              *curr = i;
            }
//           break;
          }
        }
 // Serial.print("jtag setup end");
}

//-------------------------------------------------------------------------------
//--------------------------------    ISR    ------------------------------------
//-------------------------------------------------------------------------------
FASTRUN void capture_data()
//void capture_data()
{
  digitalWriteFast(5, HIGH);
  last_portb = current_portb;
#if defined(__IMXRT1062__)
   current_portb = (digitalReadFast(SCL_PIN) << 2) | (digitalReadFast(SDA_PIN) << 3);
   current_portb = (digitalReadFast(SCL_PIN)? 4 : 0) | (digitalReadFast(SDA_PIN)? 8 : 0);
#else
  current_portb = GPIOB_PDIR & 12; //reads state of SDA (18) & SCL (19) at same time
#endif
  if (!bDone && last_portb != current_portb)
  {
    mult0xCCount = 0; //reset IDLE counter
    digitalWriteFast(MONITOR_OUT1, HIGH);

    //01/17/20: joepasquariello suggestion
    last_current = (last_portb << 4) | (current_portb);
    bIsStart = (last_current == 0xC4);
    bIsStop = (last_current == 0x4C);
    bIsData = (last_current == 0x04) || (last_current == 0x8C);

    if (bIsStart) //START
    {
      digitalWriteFast(MONITOR_OUT2, HIGH);
      if (bWaitingForStart)
      {
        digitalWriteFast(MONITOR_OUT3, HIGH); //start of entire capture
        bWaitingForStart = false;
      }
    }
    else if (bIsStop) //STOP
    {
      digitalWriteFast(MONITOR_OUT2, LOW);
    }

    if (!bWaitingForStart && (bIsData || bIsStart || bIsStop))
    {
      //digitalWriteFast(MONITOR_OUT3, HIGH);
      raw_data[write_idx] = last_portb;
      write_idx++;
      raw_data[write_idx] = current_portb;
      write_idx++;
      if (write_idx >= CAPTURE_ARRAY_SIZE)
      {
        bDone = true;
        digitalWriteFast(MONITOR_OUT3, LOW);
      }
    }
    digitalWriteFast(MONITOR_OUT1, LOW);
  }
  else if (!bDone && mult0xCCount < MAX_IDLE_COUNT && last_portb == 0xc && current_portb == 0xc)
  {
    mult0xCCount++;
    if (mult0xCCount >= MAX_IDLE_COUNT)
    {
      digitalWriteFast(MONITOR_OUT3, LOW);
      bDone = true;
    }
  }
 digitalWriteFast(5, LOW);
 }
//-------------------------------------------------------------------------------
//-------------------------------- END ISR    ---------------------------------
//-------------------------------------------------------------------------------

void i2cscan()
{
//  if (Serial.available())
//  Serial.print("i2cscan serial avail");
//  delay(5);
while(1)
//while (Serial)
 // {
  if (bDone)
  {
    if (write_idx > 14)
    {
      //OK, we have some data to process. IDLE detection must have been EOM
      Timer1.stop();

      unsigned long startMsec = millis();

      //Serial.printf("%lu\t %d\t", millis(), write_idx);
      //PrintNextArrayBytes(raw_data, 0, 50);
      //Serial.printf(" - %lu\n", millis());
      uint16_t numprocessed = DecodeAndPrintValidData(raw_data); //decode and print everything captured so far
      unsigned long endMsec = millis();
      Serial3.printf("%lu: processed = %d elements in %lu mSec\n\n", startMsec, numprocessed, endMsec - startMsec);

      Timer1.start();
    }

    read_idx = 0;
    bDone = false;
    mult0xCCount = 0;
    write_idx = 0;
    bWaitingForStart = true;
  }
  else
  {
    //no data to process, but don't blow prints out every mSec...
    if (mSecSinceLastWaitingPrint > WAITING_PRINT_INTERVAL_MSEC)
    {
      mSecSinceLastWaitingPrint -= WAITING_PRINT_INTERVAL_MSEC;
      Serial3.printf("%lu: Waiting for Data...\n", millis());
    }
  }
// }
}

void PrintNextArrayBytes(uint8_t* data, uint16_t startidx, uint16_t numbytes)
{
  Serial.printf("%d bytes starting at %d: ", numbytes, startidx);
  for (uint16_t i = 0; i < numbytes; i++)
  {
    Serial.printf("%x ", data[i + startidx]);
  }
}


uint16_t DecodeAndPrintValidData(byte* data)
{
  //Purpose:  decode and print I2C conversation held in raw_data array
  //Inputs:
  //  cb = 2048 element FIFO
  //Outputs:
  //  returns number of bytes processed, or -1 for failure
  //  outputs structured I2C sentence to serial monitor
  //Plan:
  //  Step1: Cull out invalid bytes
  //  Step2: Determine if there is anything to do (have to have more than one transition in FIFO)
  //  Step3: Parse transitions into I2C sentence structure
  //  Step4: Output sentence to serial monitor

  memset(valid_data, 0, VALID_DATA_ARRAY_SIZE);
#ifdef PARSE_LOOP_DEBUG
  PrintNextArrayBytes(valid_data, 0, 20); //print out first 20 bytes for verification
#endif
  numvalidbytes = RemoveInvalidBytes(raw_data, valid_data);
#ifdef PARSE_LOOP_DEBUG
  Serial.printf("Removed %d invalid bytes, leaving %d remaining\n", write_idx + 1 - numvalidbytes, numvalidbytes);
  PrintNextArrayBytes(valid_data, 0, numvalidbytes); //print out first 20 bytes of valid_data array
#endif


  if (numvalidbytes < 2)
  {
    return 0;
  }

  while (read_idx < numvalidbytes)
  {
#ifdef PARSE_LOOP_DEBUG
    Serial.printf("At top of while (read_idx < numvalidbytes): read_idx = %d\n", read_idx);
    Serial.printf("Next two bytes in valid_data are %x, %x\n", valid_data[read_idx], valid_data[read_idx + 1]);
#endif
    //Find a START sequence (0xC followed by 0x4)
    while (!IsStart(valid_data, read_idx) && read_idx < numvalidbytes)
    {
      //Serial.printf("looking for start...\n");
      read_idx++;
    }
    //at this point, read_idx should point to next valid byte pair

#ifdef PARSE_LOOP_DEBUG
    Serial.printf("Start sequence found at %d\n", read_idx - 2);
    //PrintNextFIFOBytes(valid_data, 20);
#endif

    if (numvalidbytes - read_idx > 14)//14 entries required for 7-bit address
    {
      //Get 7-bit device address
      devAddr = Get7BitDeviceAddr(valid_data, read_idx);
#ifdef PARSE_LOOP_DEBUG
      Serial.printf("devAddr = %x\n", devAddr);
#endif
    }
    else
    {

#ifdef PARSE_LOOP_DEBUG
      Serial.printf("ran out of data at readidx = %d - exiting!\n", read_idx);
#endif
      break;
    }

    //get read/write flag  1 = Read, 0 = Write, -1 = error
    BusDir = (BUSDIR)GetReadWriteFlag(valid_data, read_idx);

#ifdef PARSE_LOOP_DEBUG
    Serial.printf("BusDir = %s\n", ((BusDir == BUSDIR::WRITE) ? "WRITE" : "READ"));
    //PrintNextFIFOBytes(valid_data, 20);
#endif

    //get ACK/NAK flag
    ACKNAKFlag = GetACKNAKFlag(valid_data, read_idx);
    numbytes = GetDataBytes(valid_data, read_idx, databytes); //terminates on a START, but the start bytes are not consumed
#ifdef PARSE_LOOP_DEBUG
    Serial.printf("Got %d bytes from GetDataBytes() --> ", numbytes);
    for (size_t i = 0; i < numbytes; i++)
    {
      Serial.printf(" %x ", databytes[i]);
    }
    Serial.printf("\n");

    //PrintNextFIFOBytes(cb_trans, 20);
#endif
    //If the bus direction is WRITE, then extract
    //    A register address for read / write
    //    zero or more additional data bytes
    if (BusDir == BUSDIR::WRITE)
    {
      regAddr = databytes[0];
#ifdef PARSE_LOOP_DEBUG
      Serial.printf("regAddr = %x, read_idx = %d\n", regAddr, read_idx);
#endif

      //check for additional data
      if (numbytes > 1)
      {
#ifdef PARSE_LOOP_DEBUG
        Serial.printf("Additional data found!\n");
        for (size_t i = 0; i < numbytes; i++)
        {
          Serial.printf("data[%d] = %x\n", i, databytes[i]);
        }
#endif
        //1st byte is register addr, subsequent bytes are data
        OutputFormattedSentence(BusDir, devAddr, regAddr, numbytes, databytes, 1);
      }
    }
    else  //all bytes are data
    {
#ifdef PARSE_LOOP_DEBUG
      Serial.printf("In data block:  got %d bytes of data\n", numbytes);
      for (size_t i = 0; i < numbytes; i++)
      {
        Serial.printf("data[%d] = %x\n", i, databytes[i]);
      }
#endif
      OutputFormattedSentence(BusDir, devAddr, regAddr, numbytes, databytes, 0);
    }
#ifdef PARSE_LOOP_DEBUG
    Serial.printf("At end of while (read_idx < numvalidbytes): read_idx = %d\n", read_idx);
#endif

  }//while (read_idx < numvalidbytes)
  return numvalidbytes;
}


#pragma region Support Functions
bool IsStart(byte* data, uint16_t& readidx)
{
  bool result = false;

  //Serial.printf("IsStart[%d] = %x, IsStart[%d] = %x\n",
  //    readidx, data[readidx], readidx + 1, data[readidx + 1]);

  if (data[readidx] == 0xC && data[readidx + 1] == 0x4)
  {
    result = true;
    readidx += 2; //bump to next byte pair
  }
  return result;
}

bool IsStop(byte* data, uint16_t& readidx)
{
  bool result = false;

  //Serial.printf("IsStop[%d] = %x, IsStop[%d] = %x\n",
  //readidx, data[readidx], readidx + 1, data[readidx + 1]);

  if (data[readidx] == 0x4 && data[readidx + 1] == 0xC)
  {
    result = true;
    readidx += 2; //bump to next byte pair
  }
  return result;
}

uint8_t Get7BitDeviceAddr(byte* data, uint16_t& readidx)
{
  //Purpose: Construct a 7-bit address starting from dataidx
  //Inputs:
  //  data = pointer to valid data array
  //  readidx = starting index of 7-bit address sequence (MSB first)
  //Outputs:
  //  returns the address as an 8-bit value with the MSB = 0, or 0x0 if unsuccessful
  //  dataidx = pointer to next data entry
  //Plan:
  //  Step1: Convert a pair of data entries into a 0 or 1
  //  Step2: Add the appropriate value to an ongoing sum
  //  Step3: return the total.
  //Notes:
  //  A '0' is coded as a 0x0 followed by a 0x4
  //  A '1' is coded as a 0x8 followed by a 0xC

  uint8_t devAddr = 0x0; //failure return value

  //Serial.printf("Get7BitDeviceAddr: readidx = %d\n",readidx);

  //devAddr is exactly 7 bits long, so 8 bits with MSB = 0
  for (size_t i = 0; i < 7; i++)
  {
    if (data[readidx] == 0x0 && data[readidx + 1] == 0x4)
    {
      readidx += 2; //advance the pointer, but don't add to sum
    }

    else if (data[readidx] == 0x8 && data[readidx + 1] == 0xC)
    {
      //Serial.printf("Get7BitDeviceAddr: '1' found at i = %d, adding %x to devAddr to get %x\n",
      //    i, 1 << (7 - i), devAddr + (1 << (7-i)));

      readidx += 2; //advance the pointer
      devAddr += (1 << (7 - i)); //add 2^(7-i) to sum
    }
  }

  devAddr = devAddr >> 1; //divide result by 2 to get 7-bit addr from 8 bits
  return devAddr;
}

int Get8BitDataByte(byte* data, uint16_t& readidx)
{
  //Purpose: Construct a 8-bit data byte starting from dataidx
  //Inputs:
  //  data = pointer to valid data array
  //  readidx = starting index of 8-bit data byte (MSB first)
  //Outputs:
  //  returns the address as an 8-bit value, or 0x0 if unsuccessful
  //  dataidx = pointer to next data entry
  //Plan:
  //  Step1: Convert a pair of data entries into a 0 or 1
  //  Step2: Add the appropriate value to an ongoing sum
  //  Step3: return the total.
  //Notes:
  //  A '0' is coded as a 0x0 followed by a 0x4
  //  A '1' is coded as a 0x8 followed by a 0xC
  //  12/29/19 - changed return val to int, so can return -1 when a 'short byte' is detected

  int dataval = 0x0; //failure return value

#ifdef GET_8BIT_DATABYTE_DEBUG
  Serial.printf("Get8BitDataByte: data[%d] = %x, data[%d] = %x\n",
                readidx, data[readidx], readidx + 1, data[readidx + 1]);
#endif

  //8 bits with MSB = 0
  int numbytes = 0;
  for (size_t i = 0; i < 8; i++)
  {
    if (data[readidx] == 0x0 && data[readidx + 1] == 0x4)
    {
      readidx += 2; //advance the pointer, but don't add to sum
      numbytes++;
    }

    else if (data[readidx] == 0x8 && data[readidx + 1] == 0xC)
    {
#ifdef GET_8BIT_DATABYTE_DEBUG
      Serial.printf("Get8BitDataByte: '1' found at i = %d, adding %x to devAddr to get %x\n",
                    i, 1 << (7 - i), dataval + (1 << (7 - i)));
#endif
      readidx += 2; //advance the pointer
      dataval += (1 << (7 - i)); //add 2^(8-i) to sum
      numbytes++;
    }
  }

#ifdef GET_8BIT_DATABYTE_DEBUG
  Serial.printf("Get8BitDataByte: numbytes = %d\n", numbytes);
#endif
  if (numbytes != 8)
  {
    dataval = -1; //error return value
  }

  return dataval;
}

int GetReadWriteFlag(byte* data, uint16_t& readidx)
{
  //Purpose: decode R/W byte pair
  //Inputs:
  //  data = pointer to valid data array
  //  readidx = index into data to start of R/W byte pair
  //Outputs:
  //  readidx = if successful, points to next byte pair in data
  //  returns 1 for Read (0x8/0xC), 0 for Write (0x0/0x4), -1 for failure
  //Notes:
  //

  //Serial.printf("GetReadWriteFlag: readidx = %d, data[readidx] = %x, data[readidx+1]= %x\n",
  //    readidx, data[readidx], data[readidx + 1]);
  int result = 0;
  if (data[readidx] == 0x8 && data[readidx + 1] == 0xC)
  {
    result = 1; //read detected
    readidx += 2; //point to next byte pair
  }

  else if (data[readidx] == 0x0 && data[readidx + 1] == 0x4)
  {
    result = 0; //write detected
    readidx += 2; //point to next byte pair
  }
  else
  {
    result = -1; //failed to detect read or write
  }

  return result;
}

int GetACKNAKFlag(byte* data, uint16_t& readidx)
{
  //Purpose: decode ACK/NAK byte pair
  //Inputs:
  //  data = pointer to valid data array
  //  readidx = index into data to start of ACK/NAK byte pair
  //Outputs:
  //  readidx = if successful, points to next byte pair in data
  //  returns 1 for NAK (0x8/0xC), 0 for ACK (0x0/0x4), -1 for failure
  //Notes:
  //

  //Serial.printf("GetACKNAKFlag: readidx = %d, data[readidx] = %x, data[readidx+1]= %x\n",
  //    readidx, data[readidx], data[readidx + 1]);
  int result = 0;
  if (data[readidx] == 0x8 && data[readidx + 1] == 0xC)
  {
    result = 1; //NAK detected
    readidx += 2; //point to next byte pair
  }

  else if (data[readidx] == 0x0 && data[readidx + 1] == 0x4)
  {
    result = 0; //ACK detected
    readidx += 2; //point to next byte pair
  }
  else
  {
    result = -1; //failed to detect ACK or NAK
  }

  return result;
}

int GetDataBytes(uint8_t* data, uint16_t& readidx, uint8_t* databytes)
{
  //Notes:
  //  01/01/2020: removed databyteidx from sig - always starts at zero

  uint16_t numbytes = 0;
  uint16_t databyte_idx = 0;

  bool StartFlag = false;
  bool StopFlag = false;

  do
  {
    int dataval = Get8BitDataByte(data, readidx);

    //watch out for 'short byte' reads
    if (dataval >= 0)
    {
      uint8_t databyte = (uint8_t)dataval;
      databytes[databyte_idx] = databyte;
      databyte_idx++;
      numbytes++;
    }

    ACKNAKFlag = GetACKNAKFlag(data, readidx);
    StartFlag = IsStart(data, readidx);
    StopFlag = IsStop(data, readidx);

#ifdef PARSE_LOOP_DEBUG
    Serial.printf("IsStart returned %d, IsStop returned %d, dataidx = %d\n",
                  StartFlag, StopFlag, readidx);
#endif

  } while (!StartFlag && !StopFlag && readidx < numvalidbytes);


  readidx -= 2;//back readidx up so loop top is positioned correctly.

  return numbytes;
}

void OutputFormattedSentence(int RW, uint8_t dev, uint8_t reg, uint8_t numbytes, uint8_t* bytearray, uint16_t startidx)
{
  Serial3.print("%lu uhg");
  Serial.printf("I2C(%x) %s %d bytes %s 0x%x... ",
                dev, (RW == 0 ? "writing" : "reading"),  numbytes - startidx, (RW == 0 ? "to" : "from"), reg);
  for (size_t i = startidx; i < numbytes; i++)
  {
    Serial.printf("0x%x ", bytearray[i]);
  }
//  Serial.printf(". Done\n");
    Serial.printf("\n");
}

uint16_t RemoveInvalidBytes(uint8_t* rawdata, uint8_t* validdata)
{
  uint16_t numvalid = 0;
  uint16_t valididx = 0;

  //Serial.printf("raw data array contains %d bytes\n", write_idx + 1);
  //PrintNextArrayBytes(raw_data, 0, 20);

  //OK, now go back through the array, excising invalid sequences
  for (uint16_t rawidx = 0; rawidx < write_idx;/*rawidx incremented internally*/)
  {
    uint8_t firstByte = raw_data[rawidx]; //get the first byte
    uint8_t secondByte = raw_data[rawidx + 1]; //get the next byte
    bool validpair =
      (
        (firstByte == 0xC && secondByte == 0x4) //START or RESTART
        || (firstByte == 0x4 && secondByte == 0xC) //STOP
        || (firstByte == 0x0 && secondByte == 0x4) //0 OR ACK
        || (firstByte == 0x8 && secondByte == 0xC) //1 or NAK
      );

    //Serial.printf("rawidx %d: Considering %x and %x: validity = %d\n",
    //rawidx, firstByte, secondByte, validpair);
    if (validpair)
    {
      //save valid bytes to valid_bytes array
      validdata[valididx] = firstByte;
      validdata[valididx + 1] = secondByte;
      numvalid += 2;
      //Serial.printf("Added %x & %x at idx = %d & %d\n", firstByte, secondByte, valididx, valididx + 1);
      //PrintNextArrayBytes(validdata,0,numvalid);
      rawidx += 2;
      valididx += 2;
    }
    else
    {
      rawidx++; //on invalid, just go to next byte
    }
  }

  return numvalid;
}
#pragma endregion Support Functions


void tap_state(String tap_state, int tck, int tms)
{
#ifdef DEBUGTAP
  Serial.print("tap_state: tms set to: ");
#endif
  int tap_state_length = tap_state.length();
  for (int i=0; i<tap_state_length; i++) {
    if (DELAY) delayMicroseconds(DELAYUS);
    digitalWrite(tck, LOW);
    digitalWrite(tms, tap_state[i] - '0'); // conv from ascii pattern
#ifdef DEBUGTAP
    Serial.print(tap_state[i] - '0',DEC);
#endif
    digitalWrite(tck, HIGH); // rising edge shifts in TMS
  }
#ifdef DEBUGTAP
  Serial.println();
#endif
}

static void pulse_tms(int tck, int tms, int s_tms)
{
  if (tck == IGNOREPIN) return;
  digitalWrite(tck, LOW);
  digitalWrite(tms, s_tms);
  digitalWrite(tck, HIGH);
}
static void pulse_tdi(int tck, int tdi, int s_tdi)
{
  if (DELAY) delayMicroseconds(DELAYUS);
  if (tck != IGNOREPIN) digitalWrite(tck, LOW);
  digitalWrite(tdi, s_tdi);
  if (tck != IGNOREPIN) digitalWrite(tck, HIGH);
}
byte pulse_tdo(int tck, int tdo)
{
  byte tdo_read;
  if (DELAY) delayMicroseconds(DELAYUS);
  digitalWrite(tck, LOW); // read in TDO on falling edge
  tdo_read = digitalRead(tdo);
  digitalWrite(tck, HIGH);
  return tdo_read;
}

void init_pins(int tck = IGNOREPIN, int tms = IGNOREPIN, int tdi = IGNOREPIN, int ntrst = IGNOREPIN)
{
#if defined(ESP8266) || defined(ESP_H)
  ESP.wdtFeed();
#endif
  // default all to INPUT state
  for (int i = 0; i < pinslen; i++) {
    pinMode(pins[i], INPUT);
    // internal pullups default to logic 1:
    if (PULLUP) digitalWrite(pins[i], HIGH);
  }
  // TCK = output
  if (tck != IGNOREPIN) pinMode(tck, OUTPUT);
  // TMS = output
  if (tms != IGNOREPIN) pinMode(tms, OUTPUT);
  // tdi = output
  if (tdi != IGNOREPIN) pinMode(tdi, OUTPUT);
  // ntrst = output, fixed to 1
  if (ntrst != IGNOREPIN) {
    pinMode(ntrst, OUTPUT);
    digitalWrite(ntrst, HIGH);
  }
}

static int check_data(char pattern[], int iterations, int tck, int tdi, int tdo,
                      int *reg_len)
{
  int i;
  int w          = 0;
  int plen       = strlen(pattern);
  char tdo_read;
  char tdo_prev;
  int nr_toggle  = 0; // count how often tdo toggled
  /* we store the last plen (<=PATTERN_LEN) bits,
   *  rcv[0] contains the oldest bit */
  char rcv[PATTERN_LEN];

  tdo_prev = '0' + (digitalRead(tdo) == HIGH);

  for(i = 0; i < iterations; i++) {

    /* output pattern and incr write index */
    pulse_tdi(tck, tdi, pattern[w++] - '0');
    if (!pattern[w])
      w = 0;

    /* read from TDO and put it into rcv[] */
    tdo_read  =  '0' + (digitalRead(tdo) == HIGH);

    nr_toggle += (tdo_read != tdo_prev);
    tdo_prev  =  tdo_read;

    if (i < plen)
      rcv[i] = tdo_read;
    else
    {
      memmove(rcv, rcv + 1, plen - 1);
      rcv[plen-1] = tdo_read;
    }

    /* check if we got the pattern in rcv[] */
    if (i >= (plen - 1) ) {
      if (!memcmp(pattern, rcv, plen)) {
        *reg_len = i + 1 - plen;
        return 1;
      }
    }
  } /* for(i=0; ... ) */

  *reg_len = 0;
  return nr_toggle > 1 ? nr_toggle : 0;
}

static void print_pins(int tck, int tms, int tdo, int tdi, int ntrst)
{
  if (ntrst != IGNOREPIN) {
    Serial.print(" ntrst:");
    Serial.print(pinnames[ntrst]);
  }
  Serial.print(" tck:");
  Serial.print(pinnames[tck]);
  Serial.print(" tms:");
  Serial.print(pinnames[tms]);
  Serial.print(" tdo:");
  Serial.print(pinnames[tdo]);
  if (tdi != IGNOREPIN) {
    Serial.print(" tdi:");
    Serial.print(pinnames[tdi]);
  }
}

static void scan()
{
  int tck, tms, tdo, tdi, ntrst;
  int checkdataret = 0;
  int len;
  int reg_len;
  printProgStr(PSTR("================================\r\n"
                    "Starting scan for pattern:"));
  Serial.println(pattern);
  for(ntrst=0;ntrst<pinslen;ntrst++) {
    for(tck=0;tck<pinslen;tck++) {
      if(tck == ntrst) continue;
      for(tms=0;tms<pinslen;tms++) {
        if(tms == ntrst) continue;
        if(tms == tck  ) continue;
        for(tdo=0;tdo<pinslen;tdo++) {
          if(tdo == ntrst) continue;
          if(tdo == tck  ) continue;
          if(tdo == tms  ) continue;
          for(tdi=0;tdi<pinslen;tdi++) {
            if(tdi == ntrst) continue;
            if(tdi == tck  ) continue;
            if(tdi == tms  ) continue;
            if(tdi == tdo  ) continue;
            if(VERBOSE) {
              print_pins(tck, tms, tdo, tdi, ntrst);
              Serial.print("    ");
            }
            init_pins(pins[tck], pins[tms], pins[tdi], pins[ntrst]);
            tap_state(TAP_SHIFTIR, pins[tck], pins[tms]);
            checkdataret = check_data(pattern, (2*PATTERN_LEN),
                            pins[tck], pins[tdi], pins[tdo], &reg_len);
            if(checkdataret == 1) {
              Serial.print("FOUND! ");
              print_pins(tck, tms, tdo, tdi, ntrst);
              Serial.print(" IR length: ");
              Serial.println(reg_len, DEC);
            }
            else if(checkdataret > 1) {
              Serial.print("active ");
              print_pins(tck, tms, tdo, tdi, ntrst);
              Serial.print("  bits toggled:");
              Serial.println(checkdataret);
            }
            else if(VERBOSE) Serial.println();
          } /* for(tdi=0; ... ) */
        } /* for(tdo=0; ... ) */
      } /* for(tms=0; ... ) */
    } /* for(tck=0; ... ) */
  } /* for(ntrst=0; ... ) */
  printProgStr(PSTR("================================\r\n"));
}

static void loopback_check()
{
  int tdo, tdi;
  int checkdataret = 0;
  int reg_len;

  printProgStr(PSTR("================================\r\n"
                    "Starting loopback check...\r\n"));
  for(tdo=0;tdo<pinslen;tdo++) {
    for(tdi=0;tdi<pinslen;tdi++) {
      if(tdi == tdo) continue;

      if(VERBOSE) {
        Serial.print(" tdo:");
        Serial.print(pinnames[tdo]);
        Serial.print(" tdi:");
        Serial.print(pinnames[tdi]);
        Serial.print("    ");
      }
      init_pins(IGNOREPIN/*tck*/, IGNOREPIN/*tms*/, pins[tdi], IGNOREPIN /*ntrst*/);
      checkdataret = check_data(pattern, (2*PATTERN_LEN), IGNOREPIN, pins[tdi], pins[tdo], &reg_len);
      if(checkdataret == 1) {
        Serial.print("FOUND! ");
        Serial.print(" tdo:");
        Serial.print(pinnames[tdo]);
        Serial.print(" tdi:");
        Serial.print(pinnames[tdi]);
        Serial.print(" reglen:");
        Serial.println(reg_len);
      }
      else if(checkdataret > 1) {
        Serial.print("active ");
        Serial.print(" tdo:");
        Serial.print(pinnames[tdo]);
        Serial.print(" tdi:");
        Serial.print(pinnames[tdi]);
        Serial.print("  bits toggled:");
        Serial.println(checkdataret);
      }
      else if(VERBOSE) Serial.println();
    }
  }
  printProgStr(PSTR("================================\r\n"));
}

static void list_pin_names()
{
  int pin;
  Serial.print("The configured pins are:\r\n");
  for(pin=0;pin<pinslen;pin++) {
    Serial.print(pinnames[pin]);
    Serial.print(" ");
  }
  Serial.println();
}

/*
 * Scan TDO for IDCODE. Handle MAX_DEV_NR many devices.
 * We feed zeros into TDI and wait for the first 32 of them to come out at TDO (after n * 32 bit).
 * As IEEE 1149.1 requires bit 0 of an IDCODE to be a "1", we check this bit.
 * We record the first bit from the idcodes into bit0.
 * (oppposite to the old code).
 * If we get an IDCODE of all ones, we assume that the pins are wrong.
 * This scan assumes IDCODE is the default DR between TDI and TDO.
 */
static void scan_idcode()
{
  int tck, tms, tdo, tdi, ntrst;
  int i, j;
  int tdo_read;
  uint32_t idcodes[MAX_DEV_NR];
  printProgStr(PSTR("================================\r\n"
                    "Starting scan for IDCODE...\r\n"
                    "(assumes IDCODE default DR)\r\n"));
  uint32_t idcode;
  for(ntrst=0;ntrst<pinslen;ntrst++) {
    for(tck=0;tck<pinslen;tck++) {
      if(tck == ntrst) continue;
      for(tms=0;tms<pinslen;tms++) {
        if(tms == ntrst) continue;
        if(tms == tck  ) continue;
        for(tdo=0;tdo<pinslen;tdo++) {
          if(tdo == ntrst) continue;
          if(tdo == tck  ) continue;
          if(tdo == tms  ) continue;
          for(tdi=0;tdi<pinslen;tdi++) {
            if(tdi == ntrst) continue;
            if(tdi == tck  ) continue;
            if(tdi == tms  ) continue;
            if(tdi == tdo  ) continue;
            if(VERBOSE) {
              print_pins(tck, tms, tdo, tdi, ntrst);
              Serial.print("    ");
            }
            init_pins(pins[tck], pins[tms], pins[tdi], pins[ntrst]);

            /* we hope that IDCODE is the default DR after reset */
            tap_state(TAP_RESET, pins[tck], pins[tms]);
            tap_state(TAP_SHIFTDR, pins[tck], pins[tms]);

            /* j is the number of bits we pulse into TDI and read from TDO */
            for(i = 0; i < MAX_DEV_NR; i++) {
              idcodes[i] = 0;
              for(j = 0; j < IDCODE_LEN;j++) {
                /* we send '0' in */
                pulse_tdi(pins[tck], pins[tdi], 0);
                tdo_read = digitalRead(pins[tdo]);
                if (tdo_read)
                  idcodes[i] |= ( (uint32_t) 1 ) << j;

                if (VERBOSE)
                  Serial.print(tdo_read,DEC);
              } /* for(j=0; ... ) */
              if (VERBOSE) {
                Serial.print(" ");
                Serial.println(idcodes[i],HEX);
              }
              /* save time: break at the first idcode with bit0 != 1 */
              if (!(idcodes[i] & 1) || idcodes[i] == 0xffffffff)
                break;
            } /* for(i=0; ...) */

            if (i > 0) {
              print_pins(tck,tms,tdo,tdi,ntrst);
              Serial.print("  devices: ");
              Serial.println(i,DEC);
              for(j = 0; j < i; j++) {
                Serial.print("  0x");
                Serial.println(idcodes[j],HEX);
              }
            } /* if (i > 0) */
          } /* for(tdo=0; ... ) */
        } /* for(tdi=0; ...) */
      } /* for(tms=0; ...) */
    } /* for(tck=0; ...) */
  } /* for(trst=0; ...) */

  printProgStr(PSTR("================================\r\n"));
}

static void shift_bypass()
{
  int tdi, tdo, tck;
  int checkdataret;
  int reg_len;

  printProgStr(PSTR("================================\r\n"
                    "Starting shift of pattern through bypass...\r\n"
                    "Assumes bypass is the default DR on reset.\r\n"
                    "Hence, no need to check for TMS. Also, currently\r\n"
                    "not checking for nTRST, which might not work\r\n"));
  for(tck=0;tck<pinslen;tck++) {
    for(tdi=0;tdi<pinslen;tdi++) {
      if(tdi == tck) continue;
      for(tdo=0;tdo<pinslen;tdo++) {
        if(tdo == tck) continue;
        if(tdo == tdi) continue;
        if(VERBOSE) {
          Serial.print(" tck:");
          Serial.print(pinnames[tck]);
          Serial.print(" tdi:");
          Serial.print(pinnames[tdi]);
          Serial.print(" tdo:");
          Serial.print(pinnames[tdo]);
          Serial.print("    ");
        }

        init_pins(pins[tck], IGNOREPIN/*tms*/,pins[tdi], IGNOREPIN /*ntrst*/);
        // if bypass is default on start, no need to init TAP state
        checkdataret = check_data(pattern, (2*PATTERN_LEN), pins[tck], pins[tdi], pins[tdo], &reg_len);
        if(checkdataret == 1) {
          Serial.print("FOUND! ");
          Serial.print(" tck:");
          Serial.print(pinnames[tck]);
          Serial.print(" tdo:");
          Serial.print(pinnames[tdo]);
          Serial.print(" tdi:");
          Serial.println(pinnames[tdi]);
        }
        else if(checkdataret > 1) {
          Serial.print("active ");
          Serial.print(" tck:");
          Serial.print(pinnames[tck]);
          Serial.print(" tdo:");
          Serial.print(pinnames[tdo]);
          Serial.print(" tdi:");
          Serial.print(pinnames[tdi]);
          Serial.print("  bits toggled:");
          Serial.println(checkdataret);
        }
        else if(VERBOSE) Serial.println();
      }
    }
  }
  printProgStr(PSTR("================================\r\n"));
}
/* ir_state()
 * Set TAP to Reset then ShiftIR.
 * Shift in state[] as IR value.
 * Switch to ShiftDR state and end.
 */

 void ir_state(String state, int tck, int tms, int tdi)
{
#ifdef DEBUGIR
  Serial.println("ir_state: set TAP to ShiftIR:");
#endif
  tap_state(TAP_SHIFTIR, tck, tms);
#ifdef DEBUGIR
  Serial.print("ir_state: pulse_tdi to: ");
#endif
  for (int i=0; i < IR_LEN; i++) {
    if (DELAY) delayMicroseconds(DELAYUS);
    // TAP/TMS changes to Exit IR state (1) must be executed
    // at same time that the last TDI bit is sent:
    if (i == IR_LEN-1) {
      digitalWrite(tms, HIGH); // ExitIR
#ifdef DEBUGIR
      Serial.print(" (will be in ExitIR after next bit) ");
#endif
    }
    pulse_tdi(tck, tdi, state[i] - '0');
#ifdef DEBUGIR
    Serial.print(state[i] - '0', DEC);
#endif
    // TMS already set to 0 "shiftir" state to shift in bit to IR
  }
#ifdef DEBUGIR
  Serial.println("\r\nir_state: Change TAP from ExitIR to ShiftDR:");
#endif
  // a reset would cause IDCODE instruction to be selected again
  tap_state("1100", tck, tms); // -1> UpdateIR -1> SelectDR -0> CaptureDR -0> ShiftDR
}
static void sample(int iterations, int tck, int tms, int tdi, int tdo, int ntrst=IGNOREPIN)
{
  printProgStr(PSTR("================================\r\n"
                    "Starting sample (boundary scan)...\r\n"));
  init_pins(tck, tms ,tdi, ntrst);

  // send instruction and go to ShiftDR
  ir_state(IR_SAMPLE, tck, tms, tdi);

  // Tell TAP to go to shiftout of selected data register (DR)
  // is determined by the instruction we sent, in our case
  // SAMPLE/boundary scan
  for (int i = 0; i < iterations; i++) {
    // no need to set TMS. It's set to the '0' state to
    // force a Shift DR by the TAP
    Serial.print(pulse_tdo(tck, tdo),DEC);
    if (i % 32  == 31 ) Serial.print(" ");
    if (i % 128 == 127) Serial.println();
  }
}
char ir_buf[IR_LEN+1];
static void brute_ir(int iterations, int tck, int tms, int tdi, int tdo, int ntrst=IGNOREPIN)
{
  printProgStr(PSTR("================================\r\n"
                    "Starting brute force scan of IR instructions...\r\n"
                    "NOTE: If Verbose mode is off output is only printed\r\n"
                    "      after activity (bit changes) are noticed and\r\n"
                    "      you might not see the first bit of output.\r\n"
                    "IR_LEN set to "));
  Serial.println(IR_LEN,DEC);

  init_pins(tck, tms ,tdi, ntrst);
  int iractive;
  byte tdo_read;
  byte prevread;
  for (uint32_t ir = 0; ir < (1UL << IR_LEN); ir++) {
    iractive=0;
    // send instruction and go to ShiftDR (ir_state() does this already)
    // convert ir to string.
    for (int i = 0; i < IR_LEN; i++)
      ir_buf[i]=bitRead(ir, i)+'0';
    ir_buf[IR_LEN]=0;// terminate
    ir_state(ir_buf, tck, tms, tdi);
    // we are now in TAP_SHIFTDR state

    prevread = pulse_tdo(tck, tdo);
    for (int i = 0; i < iterations-1; i++) {
      // no need to set TMS. It's set to the '0' state to force a Shift DR by the TAP
      tdo_read = pulse_tdo(tck, tdo);
      if (tdo_read != prevread) iractive++;

      if (iractive || VERBOSE) {
        Serial.print(prevread,DEC);
        if (i%16 == 15) Serial.print(" ");
        if (i%128 == 127) Serial.println();
      }
      prevread = tdo_read;
    }
    if (iractive || VERBOSE) {
      Serial.print(prevread,DEC);
      Serial.print("  Ir ");
      Serial.print(ir_buf);
      Serial.print("  bits changed ");
      Serial.println(iractive, DEC);
    }
  }
}

void set_pattern()
{
  int i;
  char c;

  Serial.print("Enter new pattern of 1's or 0's (terminate with new line or '.'):\r\n"
               "> ");
  i = 0;
  while(1) {
    c = Serial.read();
    switch(c) {
    case '0':
    case '1':
      if(i < (PATTERN_LEN - 1) ) {
        pattern[i++] = c;
        Serial.print(c);
      }
      break;
    case '\n':
    case '\r':
    case '.': // bah. for the arduino serial console which does not pass us \n
      pattern[i] = 0;
      Serial.println();
      Serial.print("new pattern set [");
      Serial.print(pattern);
      Serial.println("]");
      return;
    }
  }
}

void configure_pins(){
  Serial.println("Available pins, the index is based on them");
  for(int pin=0;pin<pinslen;pin++) {
    Serial.print(pinnames[pin]);
    Serial.print("[");
    Serial.print(pin);
    Serial.print("]");
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("Current pin configuration");
  print_pins(TCK, TMS, TDO, TDI, TRST);
  byte * curr = NULL;
  for (int i = 0; i < 5; ++i) {
    curr = jtag_ptr_from_idx(i);
    do{
      // Print current value
      Serial.println();
      Serial.print(jtagpinnames[i]);
      Serial.print("(");
      Serial.print(*curr, DEC);
      Serial.print(") = ");
      // Read the new pin configuration
      while(!Serial.available())
            ;
      *curr = Serial.parseInt();
    } while(*curr < 0 || *curr >= pinslen );
    Serial.print(*curr);
#ifdef EEPROMSTORE
    // Save to eeprom
    EEPROM.write(i, *curr);
#endif
  }
  Serial.print("\nConfiguration saved\n");
}

// given a PROGMEM string, use Serial.print() to send it out
void printProgStr(const char *str)
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c);
}

void help()
{
  printProgStr(PSTR(
      "Short and long form commands can be used.\r\n"
      "\r\n"
      "SCANS\r\n"
      "-----\r\n"
      "s > pattern scan\r\n"
      "   Scans for all JTAG pins. Attempts to set TAP state to\r\n"
      "   DR_SHIFT and then shift the pattern through the DR.\r\n"
      "p > pattern set\r\n"
      "   currently: ["));
  Serial.print(pattern);
  printProgStr(PSTR("]\r\n"
      "\r\n"
      "i > idcode scan\r\n"
      "   Assumes IDCODE is default DR on reset. Ignores TDI.\r\n"
      "   Sets TAP state to DR_SHIFT and prints TDO to console\r\n"
      "   when TDO appears active. Human examination required to\r\n"
      "   determine if actual IDCODE is present. Run several\r\n"
      "   times to check for consistancy or compare against\r\n"
      "   active tdo lines found with loopback test.\r\n"
      "\r\n"
      "b > bypass scan\r\n"
      "   Assumes BYPASS is default DR on reset. Ignores TMS and\r\n"
      "   shifts pattern[] through TDI/TDO using TCK for clock.\r\n"
      "\r\n"
      "ERATTA\r\n"
      "------\r\n"
      "l > loopback check\r\n"
      "   ignores tck,tms. if patterns passed to tdo pins are\r\n"
      "   connected there is a short or a false-possitive\r\n"
      "   condition exists that should be taken into account\r\n"
      "r > pullups\r\n"
      "   internal pullups on inputs, on/off. might increase\r\n"
              "   stability when using a bad patch cable.\r\n"
      "v > verbose\r\n"
      "   on/off. print tdo bits to console during testing. If on, this will slow\r\n"
      "   down scan.\r\n"
      "d > delay\r\n"
      "   on/off. will slow down scan.\r\n"
      "- > delay -\r\n"
      "   reduce delay microseconds\r\n"
      "c > configure pin\r\n"
      "   configure jtag pins\r\n"
      "- > delay -\r\n"
      "   reduce delay microseconds\r\n"
      "+ > delay +\r\n"
      "h > help\r\n"
      "n > list pin names\r\n"
      "\r\n"
      "OTHER JTAG TESTS\r\n"
      "----------------\r\n"
      "Each of the following will not scan/find JTAG and require\r\n"
      "that you manually set the JTAG pins. See their respective\r\n"
      "call from the loop() function of code to set.\r\n"
      "\r\n"
      "1 > pattern scan single\r\n"
      "   runs a full check on one code-defined tdi<>tdo pair.\r\n"
      "   look at the main()/loop() code to specify pins.\r\n"
      "x > boundary scan\r\n"
      "   checks code defined tdo for 4000+ bits.\r\n"
      "   look at the main()/loop() code to specify pins.\r\n"
      "y > irenum\r\n"
      "   sets every possible Instruction Register and then\r\n"
      "   checks the output of the Data Register.\r\n"
      "   look at the main()/loop() code to specify pins.\r\n"
      ));
}
/*
 * main()
 */
#define CMDLEN 20
char command[CMDLEN];
int dummy;

void jtagenum()
{
//  delay(100);
  if (Serial.available())
//  delay(5);
//while(1)
//while (Serial)
  {
    //Serial.println("jtagenum functoin");
    // READ COMMAND
    delay(5); // hoping read buffer is idle after 5 ms
    int i = 0;
    while (Serial.available() && i < CMDLEN-1)
      command[i++] = Serial.read();

    Serial.flush();
    command[i] = 0; // terminate string
    Serial.println(command); // echo back

    // EXECUTE COMMAND
    if     (strcmp(command, "pattern scan") == 0                     || strcmp(command, "s") == 0)
      scan();
    else if(strcmp(command, "pattern scan single") == 0              || strcmp(command, "1") == 0)
    {
      Serial.print("pins");
      print_pins(TCK, TMS, TDO, TDI, TRST);
      init_pins(pins[TCK], pins[TMS], pins[TDI], pins[TRST] /*ntrst*/);
      tap_state(TAP_SHIFTIR, pins[TCK], pins[TMS]);
      if (check_data(pattern, (2*PATTERN_LEN), pins[TCK], pins[TDI], pins[TDO], &dummy))
        Serial.println("found pattern or other");
      else
        Serial.println("no pattern found");
    }
    else if(strcmp(command, "pattern set") == 0                      || strcmp(command, "p") == 0)
      set_pattern();
    else if(strcmp(command, "loopback check") == 0                   || strcmp(command, "l") == 0)
      loopback_check();
    else if(strcmp(command, "idcode scan") == 0                      || strcmp(command, "i") == 0)
      scan_idcode();
    else if(strcmp(command, "bypass scan") == 0                      || strcmp(command, "b") == 0)
      shift_bypass();
    else if(strcmp(command, "boundary scan") == 0                    || strcmp(command, "x") == 0)
    {
      Serial.print("pins");
      print_pins(TCK, TMS, TDO, TDI, TRST);
      Serial.println();
      sample(SCAN_LEN+100, pins[TCK], pins[TMS], pins[TDI], pins[TDO], pins[TRST]);
    }
    else if(strcmp(command, "irenum") == 0                           || strcmp(command, "y") == 0)
      brute_ir(SCAN_LEN,   pins[TCK], pins[TMS], pins[TDI], pins[TDO], pins[TRST]);
    else if(strcmp(command, "verbose") == 0                          || strcmp(command, "v") == 0)
    {
      if (VERBOSE == FALSE) {VERBOSE = TRUE;} else {VERBOSE = FALSE;}
      Serial.println(VERBOSE ? "Verbose ON" : "Verbose OFF");
    }
    else if(strcmp(command, "delay") == 0                            || strcmp(command, "d") == 0)
    {
      if (DELAY == FALSE) {DELAY = TRUE;} else {DELAY = FALSE;}
      Serial.println(DELAY ? "Delay ON" : "Delay OFF");
    }
    else if(strcmp(command, "delay -") == 0                          || strcmp(command, "-") == 0)
    {
      Serial.print("Delay microseconds: ");
      if (DELAYUS != 0 && DELAYUS > 1000) DELAYUS-=1000;
      else if (DELAYUS != 0 && DELAYUS > 100) DELAYUS-=100;
      else if (DELAYUS != 0) DELAYUS-=10;
      Serial.println(DELAYUS,DEC);
    }
    else if(strcmp(command, "delay +") == 0                          || strcmp(command, "+") == 0)
    {
      Serial.print("Delay microseconds: ");
      if (DELAYUS < 100) DELAYUS+=10;
      else if (DELAYUS <= 1000) DELAYUS+=100;
      else DELAYUS+=1000;
      Serial.println(DELAYUS,DEC);
    }
    else if(strcmp(command, "pullups") == 0                          || strcmp(command, "r") == 0)
    {
      if (PULLUP == FALSE) {PULLUP = TRUE;} else {PULLUP = FALSE;}
      Serial.println(PULLUP ? "Pullups ON" : "Pullups OFF");
    }
    else if(strcmp(command, "help") == 0                             || strcmp(command, "h") == 0)
      help();
    else if(strcmp(command, "list pin names") == 0                   || strcmp(command, "n") == 0)
      list_pin_names();
    else if(strcmp(command, "configure pin") ==  0                   || strcmp(command, "c") == 0)
      configure_pins();
    else
    {
      Serial.println("unknown command");
//      help();
    }
    Serial.print("\n> ");
  }
}
