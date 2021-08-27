
/*

  Copyright (C) Duncan Greenwood 2017 (duncan_greenwood@hotmail.com)

  This work is licensed under the:
      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   To view a copy of this license, visit:
      http://creativecommons.org/licenses/by-nc-sa/4.0/
   or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

   License summary:
    You are free to:
      Share, copy and redistribute the material in any medium or format
      Adapt, remix, transform, and build upon the material

    The licensor cannot revoke these freedoms as long as you follow the license terms.

    Attribution : You must give appropriate credit, provide a link to the license,
                  and indicate if changes were made. You may do so in any reasonable manner,
                  but not in any way that suggests the licensor endorses you or your use.

    NonCommercial : You may not use the material for commercial purposes. **(see note below)

    ShareAlike : If you remix, transform, or build upon the material, you must distribute
                 your contributions under the same license as the original.

    No additional restrictions : You may not apply legal terms or technological measures that
                                 legally restrict others from doing anything the license permits.

   ** For commercial use, please contact the original copyright holder(s) to agree licensing terms

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE

*/

// 3rd party libraries
// #include <Streaming.h>

// CBUS library
#include <CBUS.h>

// CBUS configuration object, declared externally
extern CBUSConfig config;

//
/// register the user handler for learned events
//

void CBUS::setEventHandler(void (*fptr)(byte index, CANFrame *msg)) {
  eventhandler = fptr;
}

//
/// register the user handler for CAN frames
/// note overloaded version with user array of opcodes to match
//

void CBUS::setFrameHandler(void (*fptr)(CANFrame *msg)) {
  framehandler = fptr;
  _opcodes = NULL;
  _num_opcodes = 0;
}

void CBUS::setFrameHandler(void (*fptr)(CANFrame *msg), byte *opcodes, byte num_opcodes) {
  framehandler = fptr;
  _opcodes = opcodes;
  _num_opcodes = num_opcodes;
}

//
/// assign the module parameter set
//

void CBUS::setParams(unsigned char *mparams) {
  _mparams = mparams;
}

//
/// assign the module name
//

void CBUS::setName(unsigned char *mname) {
  _mname = mname;
}

//
/// set module to SLiM mode
//

void CBUS::setSLiM(void) {

  bModeChanging = false;
  config.setNodeNum(0);
  config.setFLiM(false);
  config.setCANID(0);

  if (UI) {
    indicateMode(config.FLiM);
  }
}

//
/// extract CANID from CAN frame header
//

byte CBUS::getCANID(unsigned long header) {

  return header & 0x7f;
}

//
/// send a WRACK (write acknowledge) message
//

bool CBUS::sendWRACK(void) {

  // send a write acknowledgement response

  _msg.len = 3;
  _msg.data[0] = OPC_WRACK;
  _msg.data[1] = highByte(config.nodeNum);
  _msg.data[2] = lowByte(config.nodeNum);

  return sendMessage(&_msg);
}

//
/// send a CMDERR (command error) message
//

bool CBUS::sendCMDERR(byte cerrno) {

  // send a command error response

  _msg.len = 4;
  _msg.data[0] = OPC_CMDERR;
  _msg.data[1] = highByte(config.nodeNum);
  _msg.data[2] = lowByte(config.nodeNum);
  _msg.data[3] = cerrno;

  return sendMessage(&_msg);
}

//
/// is this an Extended CAN frame ?
//

bool CBUS::isExt(CANFrame *amsg) {

  return (amsg->ext);
}

//
/// is this a Remote frame ?
//

bool CBUS::isRTR(CANFrame *amsg) {

  return (amsg->rtr);
}

//
/// if in FLiM mode, initiate a CAN ID enumeration cycle
//

void CBUS::CANenumeration(void) {

  // initiate CAN bus enumeration cycle, either due to ENUM opcode, ID clash, or user button press

  // Serial << F("> beginning self-enumeration cycle") << endl;

  // set global variables
  bCANenum = true;                  // we are enumerating
  CANenumTime = millis();           // the cycle start time
  bCANenumComplete = false;         // the 100ms cycle has not completed
  selected_id = 1;
  enums = 0;                        // number of zero-length messages received

  // clear the results array (16 bytes * 8 bits = 128 bits)
  for (byte i = 0; i < 16; i++) {
    enum_responses[i] = 0;
  }

  // send zero-length RTR frame
  _msg.len = 0;
  sendMessage(&_msg, true, false);          // fixed arg order in v 1.1.4, RTR - true, ext = false

  // Serial << F("> enumeration cycle initiated") << endl;
  return;
}

//
/// initiate the transition from SLiM to FLiM mode
//

void CBUS::initFLiM(void) {

  // Serial << F("> initiating FLiM negotation") << endl;

  if (UI) {
    indicateMode(MODE_CHANGING);
  }

  bModeChanging = true;
  timeOutTimer = millis();

  // send RQNN message with current NN, which may be zero if a virgin/SLiM node
  _msg.len = 3;
  _msg.data[0] = OPC_RQNN;
  _msg.data[1] = highByte(config.nodeNum);
  _msg.data[2] = lowByte(config.nodeNum);
  sendMessage(&_msg);

  // Serial << F("> requesting NN with RQNN message for NN = ") << config.nodeNum << endl;
  return;
}

//
/// revert from FLiM to SLiM mode
//

void CBUS::revertSLiM(void) {

  // Serial << F("> reverting to SLiM mode") << endl;

  // send NNREL message
  _msg.len = 3;
  _msg.data[0] = OPC_NNREL;
  _msg.data[1] = highByte(config.nodeNum);
  _msg.data[2] = lowByte(config.nodeNum);

  sendMessage(&_msg);
  setSLiM();
  return;
}

//
/// change or re-confirm node number
//

void CBUS::renegotiate(void) {

  initFLiM();
}

//
/// assign the two CBUS LED objects
//

void CBUS::setLEDs(CBUSLED green, CBUSLED yellow) {

  UI = true;
  _ledGrn = green;
  _ledYlw = yellow;

  return;
}

//
/// assign the CBUS pushbutton switch object
//

void CBUS::setSwitch(CBUSSwitch sw) {

  UI = true;
  _sw = sw;
}

//
/// set the CBUS LEDs to indicate the current mode
//

void CBUS::indicateMode(byte mode) {

  // Serial << F("> indicating mode = ") << mode << endl;

  if (UI) {
    switch (mode) {

    case MODE_FLIM:
      _ledYlw.on();
      _ledGrn.off();
      break;

    case MODE_SLIM:
      _ledYlw.off();
      _ledGrn.on();
      break;

    case MODE_CHANGING:
      _ledYlw.blink();
      _ledGrn.off();
      break;

    default:
      break;
    }
  }
}

/// main CBUS message processing procedure

void CBUS::process(void) {

  byte remoteCANID = 0, nvindex = 0, nvval = 0, evnum = 0, evindex = 0, evval = 0;
  byte tarray[4];
  unsigned int nn = 0, en = 0, j = 0, opc;

  // start bus enumeration if required
  if (enumeration_required) {
    enumeration_required = false;
    CANenumeration();
  }

  // process switch operations if the module is configured with one

  if (UI) {

    // allow LEDs to update
    _ledGrn.run();
    _ledYlw.run();

    // allow the CBUS switch some processing time
    _sw.run();

    //
    /// use LEDs to indicate that the user can release the switch
    //

    if (_sw.isPressed() && _sw.getCurrentStateDuration() > SW_TR_HOLD) {
      indicateMode(MODE_CHANGING);
    }

    //
    /// handle switch state changes
    //

    if (_sw.stateChanged()) {

      // has switch been released ?
      if (!_sw.isPressed()) {

        // how long was it pressed for ?
        unsigned long press_time = _sw.getLastStateDuration();

        // long hold > 6 secs
        if (press_time > SW_TR_HOLD) {
          // initiate mode change
          if (!config.FLiM) {
            initFLiM();
          } else {
            revertSLiM();
          }
        }

        // short 1-2 secs
        if (press_time >= 1000 && press_time < 2000) {
          renegotiate();
        }

        // very short < 0.5 sec
        if (press_time < 500 && config.FLiM) {
          CANenumeration();
        }

      } else {
        // do any switch release processing here
      }
    }
  }

  // get received CAN frames from buffer
  // process at most 5 messages per run so we don't become unresponsive under load

  byte mcount = 0;

  while (available() && mcount < 5) {

    ++mcount;

    // at least one CAN frame is available in the reception buffer
    // retrieve the next one

    // memset(&_msg, 0, sizeof(CANFrame));
    _msg = getNextMessage();

    // extract OPC, NN, EN
    opc = _msg.data[0];
    nn = (_msg.data[1] << 8) + _msg.data[2];
    en = (_msg.data[3] << 8) + _msg.data[4];

    //
    /// extract the CANID of the sending module
    //

    remoteCANID = getCANID(_msg.id);

    //
    /// if registered, call the user handler with this new frame
    //

    if (framehandler != NULL) {

      // check if incoming opcode is in the user list, if list length > 0
      if (_num_opcodes > 0) {
        for (byte i = 0; i < _num_opcodes; i++) {
          if (opc == _opcodes[i]) {
            (void)(*framehandler)(&_msg);
            break;
          }
        }
      } else {
        (void)(*framehandler)(&_msg);
      }
    }

    //
    /// pulse the green LED
    //

    if (UI) {
      _ledGrn.pulse();
    }

    // is this a CANID enumeration request from another node (RTR set) ?
    if (_msg.rtr) {
      // Serial << F("> CANID enumeration RTR from CANID = ") << remoteCANID << endl;
      // send an empty message to show our CANID
      _msg.len = 0;
      sendMessage(&_msg);
      continue;
    }

    //
    /// set flag if we find a CANID conflict with the frame's producer
    /// doesn't apply to RTR or zero-length frames, so as not to trigger an enumeration loop
    //

    if (remoteCANID == config.CANID && _msg.len > 0) {
      // Serial << F("> CAN id clash, enumeration required") << endl;
      enumeration_required = true;
    }

    // is this an extended frame ? we currently ignore these as bootloader, etc data may confuse us !
    if (_msg.ext) {
      // Serial << F("> extended frame ignored, from CANID = ") << remoteCANID << endl;
      continue;
    }

    // are we enumerating CANIDs ?
    if (bCANenum && !bCANenumComplete) {

      //  a frame with zero-length message is an ENUM response
      if (_msg.len == 0) {

        // enumeratiom timer is still running -- process the CANID of this frame
        // Serial << F("> zero - length frame from CANID = ") << remoteCANID << endl;
        ++enums;

        // is there a clash with my current CANID ?
        // ignore - the module will choose an unused one at the end of enumeration
        // if (remoteCANID == config.CANID) {
        // Serial << F("> !!! there was a clash with my current CANID !!!") << endl;
        // }

        // store this response in the responses array
        if (remoteCANID > 0) {
          bitWrite(enum_responses[(remoteCANID / 8)], remoteCANID % 8, 1);
          // Serial << F("> stored CANID ") << remoteCANID << F(" at index = ") << (remoteCANID / 8) << F(", bit = ") << (remoteCANID % 8) << endl;
        }

        continue;
      }
    }

    //
    /// process the message opcode
    /// if we got this far, it's a standard CAN frame (not extended, not RTR) with a data payload length > 0
    //

    if (_msg.len > 0) {

      byte index;

      switch (opc) {

      case OPC_ACON:
      case OPC_ACON1:
      case OPC_ACON2:
      case OPC_ACON3:

      case OPC_ACOF:
      case OPC_ACOF1:
      case OPC_ACOF2:
      case OPC_ACOF3:

      case OPC_ASON:
      case OPC_ASON1:
      case OPC_ASON2:
      case OPC_ASON3:

      case OPC_ASOF:
      case OPC_ASOF1:
      case OPC_ASOF2:
      case OPC_ASOF3:

        // accessory on or off
        // try to find a matching stored event -- match on: nn, en

        index = config.findExistingEvent(nn, en);

        if (index < config.EE_MAX_EVENTS) {
          // do the module-specific action for a CBUS accessory on/off message
          if (eventhandler != NULL) {
            (void)(*eventhandler)(index, &_msg);
          }
        }

        break;

      case OPC_RQNP:
        // RQNP message - request for node paramters -- does not contain a NN or EN, so only respond if we
        // are in transition to FLiM
        // Serial << F("> RQNP -- request for node params during FLiM transition for NN = ") << nn << endl;

        // only respond if we are in transition to FLiM mode
        if (bModeChanging == true) {

          // Serial << F("> responding to RQNP with PARAMS") << endl;

          // respond with PARAMS message
          _msg.len = 8;
          _msg.data[0] = OPC_PARAMS;    // opcode
          _msg.data[1] = _mparams[1];     // manf code -- MERG
          _msg.data[2] = _mparams[2];     // minor code ver
          _msg.data[3] = _mparams[3];     // module ident
          _msg.data[4] = _mparams[4];     // number of events
          _msg.data[5] = _mparams[5];     // events vars per event
          _msg.data[6] = _mparams[6];     // number of NVs
          _msg.data[7] = _mparams[7];     // major code ver
          // final param[8] = node flags is not sent here as the max message payload is 8 bytes (0-7)
          sendMessage(&_msg);

        }

        break;

      case OPC_RQNPN:
        // RQNPN message -- request parameter by index number
        // index 0 = number of params available;
        // respond with PARAN

        if (nn == config.nodeNum) {

          byte paran = _msg.data[3];

          // Serial << F("> RQNPN request for parameter # ") << paran << F(", from nn = ") << nn << endl;

          if (paran <= _mparams[0]) {

            paran = _msg.data[3];

            _msg.len = 5;
            _msg.data[0] = OPC_PARAN;
            _msg.data[1] = highByte(config.nodeNum);
            _msg.data[2] = lowByte(config.nodeNum);
            _msg.data[3] = paran;
            _msg.data[4] = _mparams[paran];
            sendMessage(&_msg);

          } else {
            // Serial << F("> RQNPN - param #") << paran << F(" is out of range !") << endl;
            sendCMDERR(9);
          }
        }

        break;

      case OPC_SNN:
        // received SNN - set node number
        // Serial << F("> received SNN with NN = ") << nn << endl;

        if (bModeChanging) {
          // Serial << F("> buf[1] = ") << _msg.data[1] << ", buf[2] = " << _msg.data[2] << endl;

          // save the NN
          config.setNodeNum((_msg.data[1] << 8) + _msg.data[2]);

          // respond with NNACK
          _msg.len = 3;
          _msg.data[0] = OPC_NNACK;
          _msg.data[1] = highByte(config.nodeNum);
          _msg.data[2] = lowByte(config.nodeNum);

          sendMessage(&_msg);

          // Serial << F("> sent NNACK for NN = ") << config.nodeNum << endl;

          // we are now in FLiM mode - update the configuration
          bModeChanging = false;
          config.setFLiM(true);

          if (UI) {
            indicateMode(config.FLiM);
          }

          // enumerate the CAN bus to allocate a free CAN ID
          CANenumeration();

          // Serial << F("> FLiM mode = ") << config.FLiM << F(", node number = ") << config.nodeNum << F(", CANID = ") << config.CANID << endl;

        } else {
          // Serial << F("> received SNN but not in transition") << endl;
        }

        break;

      case OPC_CANID:
        // CAN -- set CANID
        // Serial << F("> CANID for nn = ") << nn << F(" with new CANID = ") << msg.data[3] << endl;

        if (nn == config.nodeNum) {
          // Serial << F("> setting my CANID to ") << CANID << endl;
          if (_msg.data[3] < 1 || _msg.data[3] > 99) {
            sendCMDERR(7);
          } else {
            config.setCANID(_msg.data[3]);
          }
        }

        break;

      case OPC_ENUM:
        // received ENUM -- start CAN bus self-enumeration
        // Serial << F("> ENUM message for nn = ") << nn << F(" from CANID = ") << remoteCANID << endl;

        if (nn == config.nodeNum && remoteCANID != config.CANID && !bCANenum) {
          CANenumeration();
        }

        break;

      case OPC_NVRD:
        // received NVRD -- read NV by index
        if (nn == config.nodeNum) {

          nvindex = _msg.data[3];
          // Serial << F("> NVRD for nn = ") << nn << F(", nv index = ") << nvindex << endl;

          if (nvindex > config.EE_NUM_NVS) {
            sendCMDERR(10);
          } else {
            // respond with NVANS
            _msg.len = 5;
            _msg.data[0] = OPC_NVANS;
            _msg.data[1] = highByte(config.nodeNum);
            _msg.data[2] = lowByte(config.nodeNum);
            _msg.data[3] = nvindex;
            _msg.data[4] = config.readNV(nvindex);
            sendMessage(&_msg);
          }
        }

        break;

      case OPC_NVSET:
        // received NVSET -- set NV by index
        if (nn == config.nodeNum) {

          nvindex = _msg.data[3];
          nvval = _msg.data[4];
          // Serial << F("> NVSET for index = ") << nvindex << F(", val = ") << nvval << endl;

          if (nvindex > config.EE_NUM_NVS) {
            sendCMDERR(10);
          } else {
            // update EEPROM for this NV -- NVs are indexed from 1, not zero
            config.writeNV(nvindex, nvval);
            // respond with WRACK
            sendWRACK();
          }
        }

        break;

      case OPC_NNLRN:
        // received NNLRN -- place into learn mode
        if (nn == config.nodeNum) {
          // Serial << F("> NNLRN for node = ") << nn << F(", learn mode on") << endl;
          bLearn = true;
          // set bit 5 in parameter 8
          bitSet(_mparams[8], 5);
        }

        break;

      case OPC_EVULN:
        // received EVULN -- unlearn an event, by event number
        // en = (_msg.data[3] << 8) + _msg.data[4];
        // Serial << F("> EVULN for nn = ") << nn << F(", en = ") << en << endl;

        // we must be in learn mode
        if (bLearn == true) {

          // Serial << F("> searching for existing event to unlearn") << endl;

          // search for this NN and EN pair
          index = config.findExistingEvent(nn, en);

          if (index < config.EE_MAX_EVENTS) {

            // Serial << F("> deleting event at index = ") << index << F(", evs ") << endl;
            config.cleareventEEPROM(j);

            // update hash table
            config.updateEvHashEntry(j);

            // respond with WRACK
            sendWRACK();

          } else {
            // Serial << F("> did not find event to delete") << endl;
            // respond with CMDERR
            sendCMDERR(10);
          }

        } // if in learn mode

        break;

      case OPC_NNULN:
        // received NNULN -- exit from learn mode

        if (nn == config.nodeNum) {
          bLearn = false;
          // Serial << F("> NNULN for node = ") << nn << F(", learn mode off") << endl;
          // clear bit 5 in parameter 8
          bitClear(_mparams[8], 5);
        }

        break;

      case OPC_RQEVN:
        // received RQEVN -- request for number of stored events
        // Serial << F("> RQEVN -- number of stored events for nn = ") << nn << endl;

        if (nn == config.nodeNum) {

          evnum = config.numEvents();
          // Serial << F("> replying to RQEVN with stored events = ") << evnum << endl;

          // respond with 0x74 NUMEV
          _msg.len = 4;
          _msg.data[0] = OPC_NUMEV;
          _msg.data[1] = highByte(config.nodeNum);
          _msg.data[2] = lowByte(config.nodeNum);
          _msg.data[3] = evnum;

          sendMessage(&_msg);
        }

        break;

      case OPC_NERD:
        // request for all stored events
        // Serial << F("> NERD : request all stored events for nn = ") << nn << endl;

        if (nn == config.nodeNum) {

          evnum = 0;
          _msg.len = 8;
          _msg.data[0] = OPC_ENRSP;                       // response opcode
          _msg.data[1] = highByte(config.nodeNum);        // my NN hi
          _msg.data[2] = lowByte(config.nodeNum);         // my NN lo

          for (byte i = 0; i < config.EE_MAX_EVENTS; i++) {

            if (config.getEvTableEntry(i) != 0) {
              // it's a valid stored event

              // read the event data from EEPROM
              config.readEvent(i, tarray);

              // construct and send a ENRSP message
              _msg.data[3] = tarray[0];     // event NNhi
              _msg.data[4] = tarray[1];     // event NNlo
              _msg.data[5] = tarray[2];     // event ENhi
              _msg.data[6] = tarray[3];     // event ENlo
              _msg.data[7] = i;             // event table index

              // Serial << F("> sending ENRSP reply for event index = ") << i << endl;
              sendMessage(&_msg);
              delay(10);

            } // valid stored ev
          } // loop each ev
        } // for me

        break;

      case OPC_REVAL:
        // received REVAL -- request read of an event variable by event index and ev num
        // respond with NEVAL

        if (nn == config.nodeNum) {

          byte eventidx = _msg.data[3];      // stored event index, from 0
          byte evvaridx = _msg.data[4];      // event var index, from 1

          // Serial << F("> REVAL -- request event variable for nn = ") << nn << F(", eventidx = ") << eventidx << F(", evvaridx = ") << evvaridx << endl;

          if (config.getEvTableEntry(eventidx) != 0) {

            // byte evval = ((byte)config.readEEPROM(config.EE_EVENTS_START + (eventidx * config.EE_BYTES_PER_EVENT) + 4 + (evvaridx - 1)));
            byte evval = config.getEventEVval(eventidx, evvaridx);
            // Serial << F("> evval = ") << evval << endl;

            _msg.len = 6;
            _msg.data[0] = OPC_NEVAL;
            _msg.data[1] = highByte(config.nodeNum);
            _msg.data[2] = lowByte(config.nodeNum);
            _msg.data[3] = eventidx;
            _msg.data[4] = evvaridx;
            _msg.data[5] = evval;

            sendMessage(&_msg);
          } else {

            // Serial << F("> request for invalid event index") << endl;
            sendCMDERR(6);
          }

        }

        break;

      case OPC_NNCLR:
        // NNCLR -- clear all stored events

        if (bLearn == true && nn == config.nodeNum) {

          // Serial << F("> NNCLR -- clear all events") << endl;

          for (byte e = 0; e < config.EE_MAX_EVENTS; e++) {
            config.cleareventEEPROM(e);
          }

          // recreate the hash table
          config.clearEvHashTable();
          // Serial << F("> cleared all events") << endl;

          sendWRACK();
        }

        break;

      case OPC_NNEVN:
        // request for number of free event slots

        if (config.nodeNum == nn) {

          byte free_slots = 0;

          for (byte i = 0; i < config.EE_MAX_EVENTS; i++) {
            if (config.getEvTableEntry(i) == 0) {
              ++free_slots;
            }
          }

          // Serial << F("> responding to to NNEVN with EVNLF, free event table slots = ") << free_slots << endl;
          // memset(&_msg, 0, sizeof(_msg));
          _msg.len = 4;
          _msg.data[0] = OPC_EVNLF;
          _msg.data[1] = highByte(config.nodeNum);
          _msg.data[2] = lowByte(config.nodeNum);
          _msg.data[3] = free_slots;
          sendMessage(&_msg);
        }

        break;

      case OPC_QNN:
        // this is probably a config recreate -- respond with PNN if we have a node number
        // Serial << F("> QNN received") << endl;

        if (config.nodeNum > 0) {
          // Serial << ("> responding with PNN message") << endl;
          _msg.len = 6;
          _msg.data[0] = OPC_PNN;
          _msg.data[1] = highByte(config.nodeNum);
          _msg.data[2] = lowByte(config.nodeNum);
          _msg.data[3] = _mparams[1];
          _msg.data[4] = _mparams[3];
          _msg.data[5] = _mparams[8];
          sendMessage(&_msg);
        }

        break;

      case OPC_RQMN:
        // request for node module name, excluding "CAN" prefix
        // sent during module transition, so no node number check
        // Serial << F("> RQMN received") << endl;

        // only respond if in transition to FLiM

        // respond with NAME
        if (bModeChanging) {
          _msg.len = 8;
          _msg.data[0] = OPC_NAME;
          memcpy(_msg.data + 1, _mname, 7);
          sendMessage(&_msg);
        }

        break;

      case OPC_EVLRN:
        // received EVLRN -- learn an event
        evindex = _msg.data[5];
        evval = _msg.data[6];

        // Serial << endl << F("> EVLRN for source nn = ") << nn << F(", en = ") << en << F(", evindex = ") << evindex << F(", evval = ") << evval << endl;

        // we must be in learn mode
        if (bLearn == true) {

          // search for this NN, EN as we may just be adding an EV to an existing learned event
          // Serial << F("> searching for existing event to update") << endl;
          index = config.findExistingEvent(nn, en);

          // not found - it's a new event
          if (index >= config.EE_MAX_EVENTS) {
            // Serial << F("> existing event not found - creating a new one if space available") << endl;
            index = config.findEventSpace();
          }

          // if existing or new event space found, write the event data

          if (index < config.EE_MAX_EVENTS) {

            // write the event to EEPROM at this location -- EVs are indexed from 1 but storage offsets start at zero !!
            // Serial << F("> writing EV = ") << evindex << F(", at index = ") << index << F(", offset = ") << (EE_EVENTS_START + (index * EE_BYTES_PER_EVENT)) << endl;

            // don't repeat this for subsequent EVs
            if (evindex < 2) {
              tarray[0] = highByte(nn);
              tarray[1] = lowByte(nn);
              tarray[2] = highByte(en);
              tarray[3] = lowByte(en);
              config.writeEvent(index, tarray);
            }

            config.writeEventEV(index, evindex, evval);

            // recreate event hash table entry
            // Serial << F("> updating hash table entry for idx = ") << index << endl;
            config.updateEvHashEntry(index);

            // respond with WRACK
            sendWRACK();

          } else {
            // Serial << F("> no free event storage, index = ") << index << endl;
            // respond with CMDERR
            sendCMDERR(10);
          }

        } else { // bLearn == true
          // Serial << F("> error -- not in learn mode") << endl;
        }

        break;

      case OPC_AREQ:
        // AREQ message - request for node state, only producer nodes
        break;

      case OPC_BOOT:
        // boot mode
        break;

      case OPC_RSTAT:
        // command station status -- not applicable to modules
        break;

      // case OPC_ARST:
      // system reset ... this is not what I thought it meant !
      // config.reboot();
      // break;

      default:
        // unknown or unhandled OPC
        // Serial << F("> opcode 0x") << _HEX(opc) << F(" is not currently implemented")  << endl;
        break;
      }
    } else {
      // Serial << F("> oops ... zero - length frame ?? ") << endl;
    }
  }  // while messages available

  // check CAN bus enumeration timer
  checkCANenum();

  //
  /// check 30 sec timeout for SLiM/FLiM negotiation with FCU
  //

  if (bModeChanging && ((millis() - timeOutTimer) >= 30000)) {

    // Serial << F("> timeout expired, FLiM = ") << FLiM << F(", mode change = ") << bModeChanging << endl;
    if (UI) {
      indicateMode(config.FLiM);
    }

    bModeChanging = false;
  }

  // Serial << F("> end of opcode processing, time = ") << (micros() - mtime) << "us" << endl;

  //
  /// end of CBUS message processing
  //

  return;
}

void CBUS::checkCANenum(void) {

  //
  /// check the 100ms CAN enumeration cycle timer
  //

  if (bCANenum && !bCANenumComplete && (millis() - CANenumTime) >= 100) {

    // enumeration timer has expired -- stop enumeration and process the responses

    // Serial << F("> enum cycle complete at ") << millis() << F(", start = ") << CANenumTime << F(", duration = ") << (millis() - CANenumTime) << endl;
    // Serial << F("> processing received responses") << endl;

    // iterate through the 128 bit field
    for (byte i = 0; i < 16; i++) {

      // for each bit in the byte
      for (byte b = 0; b < 8; b++) {

        // ignore first bit of first byte -- CAN ID zero is not used for nodes
        if (i == 0 && b == 0) {
          continue;
        }

        // ignore if this byte is all 1's -> there are no unused IDs in this group of numbers
        if (enum_responses[i] == 0xff) {
          continue;
        }

        // if the bit is not set
        if (bitRead(enum_responses[i], b) == 0) {
          selected_id = ((i * 16) + b);
          // Serial << F("> bit ") << b << F(" of byte ") << i << F(" is not set, first free CAN ID = ") << selected_id << endl;
          i = 16; // ugh ... but probably better than a goto :)
          break;
        }
      }
    }

    // Serial << F("> enumeration responses = ") << enums << F(", lowest available CAN id = ") << selected_id << endl;

    bCANenumComplete = true;
    bCANenum = false;
    CANenumTime = 0UL;

    // store the new CAN ID
    config.setCANID(selected_id);

    // send NNACK
    _msg.len = 3;
    _msg.data[0] = OPC_NNACK;
    _msg.data[1] = highByte(config.nodeNum);
    _msg.data[2] = lowByte(config.nodeNum);
    sendMessage(&_msg);
  }
}

//
/// utility to create a CAN header
//

void CBUS::makeHeader(CANFrame *msg) {

  uint16_t t = 0;

  // set the CANID
  t = config.CANID;

  // set the CBUS message priority - zeroes equate to higher priority
  // bits 7 and 8 are the minor priority, so 11 = 'low'
  bitSet(t, 7);
  bitSet(t, 8);

  // bits 9 and 10 are the major priority, so 01 = 'medium'
  bitClear(t, 9);
  bitSet(t, 10);

  // copy the temporary value
  msg->id = t;
}

//
/// constructor for CANFrame class
//

CANFrame::CANFrame() {

  // set the current CAN ID in the frame header
  id = config.CANID;

  // set the CBUS message priority - zeroes equate to higher priority
  // bits 7 and 8 are the minor priority, so 11 = 'low'
  bitSet(id, 7);
  bitSet(id, 8);

  // bits 9 and 10 are the major priority, so 01 = 'medium'
  bitClear(id, 9);
  bitSet(id, 10);

  ext = false;
  rtr = false;
  len = 0;
}
