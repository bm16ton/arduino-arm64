/**
 * CAN MCP2515_nb
 * Copyright 2020 WitchCraftWorks Team, All Rights Reserved
 *
 * Licensed under Apache 2.0
 */

#include "MCP2515_nb.h"

MCP2515 MCP = MCP2515();

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    Serial.println("CAN Receiver Callback");

    // start the CAN bus at 50 kbps
    if (!MCP.begin(50E3)) {
        Serial.println("Starting CAN failed!");
        while (true);
    }
}

void loop() {
    Serial.println("Sending 11 bit standard packet");

    CANPacket std = CANPacket();

    std.startStandard(0x80);
    std.writeData('a'); // up to 8 data bytes
    std.writeData('b');
    std.writeData('c');
    std.writeData('d');
    std.end();

    int result = MCP.writePacket(&std);
    Serial.print("Write standard package error code: ");
    Serial.println(result, DEC);

    Serial.println("Sending 29 bit extended packet");

    CANPacket ext = CANPacket();

    std.startExtended(0xABCDEF);
    ext.writeData('h'); // up to 8 data bytes
    ext.writeData('e');
    ext.writeData('l');
    ext.writeData('l');
    ext.writeData('0');
    ext.end();

    int result2 = MCP.writePacket(&ext);
    Serial.print("Write extended package error code: ");
    Serial.println(result2, DEC);

    delay(1000);
}
