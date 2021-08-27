/**
 * CAN MCP2515_nb
 * Copyright 2020 WitchCraftWorks Team, All Rights Reserved
 *
 * Licensed under Apache 2.0
 */

#ifndef CANPACKET_H
#define CANPACKET_H

#include <Arduino.h>
#include "MCP2515.h"

class CANPacket {
    friend class MCP2515;

public:
    static const unsigned long STATUS_RX_OK = (1 << 0);
    static const unsigned long STATUS_RX_INVALID_MESSAGE = (1 << 1);

    static const unsigned long STATUS_TX_PENDING = (1 << 4);
    static const unsigned long STATUS_TX_WRITTEN = (1 << 5); // packet written to CAN controller, confirmation pending
    static const unsigned long STATUS_TX_SENT = (1 << 7);

    static const unsigned long STATUS_TX_ABORT_REQUESTED = (1 << 10);
    static const unsigned long STATUS_TX_ABORTED = (1 << 11);

    static const unsigned long STATUS_TX_ERROR = (1 << 14);

    CANPacket();
    ~CANPacket();

    CANPacket(CANPacket&) = default;
    CANPacket(const CANPacket&) = default;

    bool isValid();
    bool isExtended();
    unsigned long getStatus();

    long getId();
    int getDlc();
    int getRtr();
    uint8_t* getData();

    int startStandard(int id, int dlc = -1, bool rtr = false);
    int startExtended(long id, int dlc = -1, bool rtr = false);

    int writeData(uint8_t byte);
    int writeData(const uint8_t* buffer, size_t size);

    int end();

private:
    bool _started = false;
    bool _ended = false;
    bool _aborted = false;
    unsigned long _status = 0;

    bool _extended = false;
    long _id = 0;
    int _dlc = -1;
    bool _rtr = false;
    uint8_t _data[8];
    int _dataLength = 0;
};

#endif
