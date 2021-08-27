/**
 * CAN MCP2515_nb
 * Copyright 2020 WitchCraftWorks Team, All Rights Reserved
 *
 * Licensed under Apache 2.0
 */

#include "CANPacket.h"

static inline __attribute__((always_inline))
int determineReturnCodeByPacketStatus(CANPacket* packet) {
    if (packet->getStatus() & CANPacket::STATUS_TX_SENT) {
        return MCP2515_ERRORCODES::OK;
    } else if (packet->getStatus() & CANPacket::STATUS_TX_ABORTED) {
        if (packet->getStatus() & CANPacket::STATUS_TX_ABORT_REQUESTED) {
            return MCP2515_ERRORCODES::OK;
        }

        return MCP2515_ERRORCODES::INTR;
    } else if (packet->getStatus() & CANPacket::STATUS_TX_ERROR) {
        return MCP2515_ERRORCODES::BADF;
    }

    return MCP2515_ERRORCODES::INVAL;
}
