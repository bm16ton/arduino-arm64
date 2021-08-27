//----------------------------------------------------------------------------------------------------------------------
// A CAN driver for MCP2517FD CAN Controller in CAN 2.0B mode
// by Pierre Molinaro
// https://github.com/pierremolinaro/acan2517
//
// This file is common with the acan2515 library
// https://github.com/pierremolinaro/acan2515
//
//----------------------------------------------------------------------------------------------------------------------

#ifndef ACAN_BUFFER_8_CLASS_DEFINED
#define ACAN_BUFFER_8_CLASS_DEFINED

//----------------------------------------------------------------------------------------------------------------------

#include <CANMessage.h>

//----------------------------------------------------------------------------------------------------------------------

class ACANBuffer8 {

//······················································································································
// Default constructor
//······················································································································

  public: ACANBuffer8 (void)  :
  mBuffer (NULL),
  mSize (0),
  mReadIndex (0),
  mCount (0),
  mPeakCount (0) {
  }

//······················································································································
// Destructor
//······················································································································

  public: ~ ACANBuffer8 (void) {
    delete [] mBuffer ;
  }

//······················································································································
// Private properties
//······················································································································

  private: CANMessage * mBuffer ;
  private: uint8_t mSize ;
  private: uint8_t mReadIndex ;
  private: uint8_t mCount ;
  private: uint16_t mPeakCount ; // > mSize if overflow did occur

//······················································································································
// Accessors
//······················································································································

  public: inline uint8_t size (void) const { return mSize ; }
  public: inline uint8_t count (void) const { return mCount ; }
  public: inline uint16_t peakCount (void) const { return mPeakCount ; }

//······················································································································
// initWithSize
//······················································································································

  public: void initWithSize (const uint8_t inSize) {
    delete [] mBuffer ;
    mBuffer = new CANMessage [inSize] ;
    mSize = inSize ;
    mReadIndex = 0 ;
    mCount = 0 ;
    mPeakCount = 0 ;
  }

//······················································································································
// append
//······················································································································

  public: bool append (const CANMessage & inMessage) {
    const bool ok = mCount < mSize ;
    if (ok) {
      uint16_t writeIndex = ((uint16_t) mReadIndex) + (uint16_t) mCount ;
      if (writeIndex >= mSize) {
        writeIndex -= mSize ;
      }
      mBuffer [writeIndex] = inMessage ;
      mCount ++ ;
      if (mPeakCount < mCount) {
        mPeakCount = mCount ;
      }
    }
    return ok ;
  }

//······················································································································
// Remove
//······················································································································

  public: bool remove (CANMessage & outMessage) {
    const bool ok = mCount > 0 ;
    if (ok) {
      outMessage = mBuffer [mReadIndex] ;
      mCount -= 1 ;
      mReadIndex += 1 ;
      if (mReadIndex == mSize) {
        mReadIndex = 0 ;
      }
    }
    return ok ;
  }

//······················································································································
// No copy
//······················································································································

  private: ACANBuffer8 (const ACANBuffer8 &) ;
  private: ACANBuffer8 & operator = (const ACANBuffer8 &) ;
} ;

//----------------------------------------------------------------------------------------------------------------------

#endif
