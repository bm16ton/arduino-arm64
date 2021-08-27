/**
 * CAN MCP2515_nb
 * Copyright 2020 WitchCraftWorks Team, All Rights Reserved
 *
 * Licensed under Apache 2.0
 */

#include "CANPacket.h"
#include "MCP2515.h"

CANPacket::CANPacket() {
    memset(_data, 0, sizeof(_data));
}

CANPacket::~CANPacket() {

}

bool CANPacket::isValid() {
    return _ended;
}

bool CANPacket::isExtended() {
    return _extended;
}

unsigned long CANPacket::getStatus() {
    return _status;
}

long CANPacket::getId() {
    return _id;
}

int CANPacket::getDlc() {
    return _dlc;
}

int CANPacket::getRtr() {
    return _rtr;
}

uint8_t* CANPacket::getData() {
    return _data;
}

int CANPacket::startStandard(int id, int dlc, bool rtr) {
    if (_started) {
        return MCP2515_ERRORCODES::PERM;
    }

    if (id < 0 || id > 0x7FF) {
        return MCP2515_ERRORCODES::INVAL;
    }

    if (dlc > 8) {
        return MCP2515_ERRORCODES::INVAL;
    }

    _started = true;
    _extended = false;

    _id = id;
    _rtr = rtr;
    _dlc = dlc;
    _dataLength = 0;

    return MCP2515_ERRORCODES::OK;
}

int CANPacket::startExtended(long id, int dlc, bool rtr) {
    if (_started) {
        return MCP2515_ERRORCODES::PERM;
    }

    if (id < 0 || id > 0x1FFFFFFF) {
        return MCP2515_ERRORCODES::INVAL;
    }

    if (dlc > 8) {
        return MCP2515_ERRORCODES::INVAL;
    }

    _started = true;
    _extended = true;

    _id = id;
    _rtr = rtr;
    _dlc = dlc;
    _dataLength = 0;

    memset(_data, 0x00, sizeof(_data));

    return MCP2515_ERRORCODES::OK;
}

int CANPacket::writeData(uint8_t data) {
    return writeData(&data, sizeof(data));
}

int CANPacket::writeData(const uint8_t* buffer, size_t size) {
    if (!_started) {
        return MCP2515_ERRORCODES::PERM;
    }

    if (_dataLength >= 8 || size > (sizeof(_data) - _dataLength)) {
        return MCP2515_ERRORCODES::INVAL;
    }

    memcpy(&_data[_dataLength], buffer, size);
    _dataLength += size;

    return MCP2515_ERRORCODES::OK;
}

int CANPacket::end() {
    if (!_started) {
        return MCP2515_ERRORCODES::PERM;
    }

    _ended = true;

    if (_dlc < 0) {
        _dlc = _dataLength;
    }

    return MCP2515_ERRORCODES::OK;
}
