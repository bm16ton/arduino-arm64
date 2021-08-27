//----------------------------------------------------------------------------------------------------------------------
// A CAN driver for MCP2515
// by Pierre Molinaro
// https://github.com/pierremolinaro/acan2515Tiny
//
//----------------------------------------------------------------------------------------------------------------------

#pragma once

//----------------------------------------------------------------------------------------------------------------------

#include <ACANBuffer8.h>
#include <ACAN2515TinySettings.h>
#include <MCP2515ReceiveFilters.h>
#include <SPI.h>

//----------------------------------------------------------------------------------------------------------------------

class ACAN2515Tiny {
//--- Constructor
  public: ACAN2515Tiny (const uint8_t inCS,  // CS input of MCP2515
                        SPIClass & inSPI, // Hardware SPI object
                        const uint8_t inINT) ; // INT output of MCP2515


//--- Initialisation: returns 0 if ok, otherwise see error codes below
  public: uint16_t begin (const ACAN2515TinySettings & inSettings,
                          void (* inInterruptServiceRoutine) (void)) ;

  public: uint16_t begin (const ACAN2515TinySettings & inSettings,
                          void (* inInterruptServiceRoutine) (void),
                          const ACAN2515Mask inRXM0,
                          const ACAN2515AcceptanceFilter inAcceptanceFilters [],
                          const uint8_t inAcceptanceFilterCount) ;

  public: uint16_t begin (const ACAN2515TinySettings & inSettings,
                          void (* inInterruptServiceRoutine) (void),
                          const ACAN2515Mask inRXM0,
                          const ACAN2515Mask inRXM1,
                          const ACAN2515AcceptanceFilter inAcceptanceFilters [],
                          const uint8_t inAcceptanceFilterCount) ;

//--- Error codes returned by begin
  public: static const uint16_t kNoMCP2515                   = 1 <<  0 ;
  public: static const uint16_t kTooFarFromDesiredBitRate    = 1 <<  1 ;
  public: static const uint16_t kInconsistentBitRateSettings = 1 <<  2 ;
  public: static const uint16_t kINTPinIsNotAnInterrupt      = 1 <<  3 ;
  public: static const uint16_t kISRIsNull                   = 1 <<  4 ;
  public: static const uint16_t kRequestedModeTimeOut        = 1 <<  5 ;
  public: static const uint16_t kAcceptanceFilterArrayIsNULL = 1 << 6 ;
  public: static const uint16_t kOneFilterMaskRequiresOneOrTwoAcceptanceFilters = 1 << 7 ;
  public: static const uint16_t kTwoFilterMasksRequireThreeToSixAcceptanceFilters = 1 << 8 ;

//--- Receiving messages
  public: bool available (void) ;
  public: bool receive (CANMessage & outFrame) ;
  public: typedef void (*tFilterMatchCallBack) (const uint8_t inFilterIndex) ;
  public: bool dispatchReceivedMessage (const tFilterMatchCallBack inFilterMatchCallBack = NULL) ;

//--- Handling messages to send and receiving messages
  public: void isr (void) ;
  private: void handleTXBInterrupt (void) ;
  private: void handleRXBInterrupt (void) ;

 //--- Properties
  private: SPIClass & mSPI ;
  private: const SPISettings mSPISettings ;
  private: const uint8_t mCS ;
  private: const uint8_t mINT ;

//--- Receive buffer
  private: ACANBuffer8 mReceiveBuffer ;

  public: inline uint8_t receiveBufferPeakCount (void) const {
    return mReceiveBuffer.peakCount () ;
  }

//--- Call back function array
  private: ACANCallBackRoutine mCallBackFunctionArray [6] ;

//--- Transmitting messages
  public: bool tryToSend (const CANMessage & inMessage) ;

//--- Driver transmit buffer
  private: ACANBuffer8 mTransmitBuffer ;
  private: bool mTXBIsFree ;

  public: inline uint8_t transmitBufferSize (void) const {
    return mTransmitBuffer.size () ;
  }

  public: inline uint8_t transmitBufferCount (void) const {
    return mTransmitBuffer.count () ;
  }

  public: inline uint8_t transmitBufferPeakCount (void) const {
    return mTransmitBuffer.peakCount () ;
  }
  private: void internalSendMessage (const CANMessage & inFrame) ;

//--- Private methods
  private: inline void select (void) { digitalWrite (mCS, LOW) ; }
  private: inline void unselect (void) { digitalWrite (mCS, HIGH) ; }

  private: uint16_t beginWithoutFilterCheck (const ACAN2515TinySettings & inSettings,
                                             void (* inInterruptServiceRoutine) (void),
                                             const ACAN2515Mask inRXM0,
                                             const ACAN2515Mask inRXM1,
                                             const ACAN2515AcceptanceFilter inAcceptanceFilters [],
                                             const uint8_t inAcceptanceFilterCount) ;

  private: uint16_t internalBeginOperation (const ACAN2515TinySettings & inSettings,
                                            const ACAN2515Mask inRXM0,
                                            const ACAN2515Mask inRXM1,
                                            const ACAN2515AcceptanceFilter inAcceptanceFilters [],
                                            const uint8_t inAcceptanceFilterCount) ;

  private: void write2515Register (const uint8_t inRegister, const uint8_t inValue) ;
  private: uint8_t read2515Register (const uint8_t inRegister) ;
  private: uint8_t read2515Status (void) ;
  private: uint8_t read2515RxStatus (void) ;
  private: void bitModify2515Register (const uint8_t inRegister, const uint8_t inMask, const uint8_t inData) ;
  private: void setupMaskRegister (const ACAN2515Mask inMask, const uint8_t inRegister) ;

//--- MCP2515 controller state
  public: uint8_t receiveErrorCounter (void) ;
  public: uint8_t transmitErrorCounter (void) ;

//--- No Copy
  private: ACAN2515Tiny (const ACAN2515Tiny &) ;
  private: ACAN2515Tiny & operator = (const ACAN2515Tiny &) ;
} ;

//----------------------------------------------------------------------------------------------------------------------
