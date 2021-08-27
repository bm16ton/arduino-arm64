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

	// register the receive callback
	MCP.onReceivePacket(onReceive);
}

void loop() {
	// do nothing
}

void onReceive(CANPacket* packet) {
	Serial.print("Received ");

	if (packet->isExtended()) {
		Serial.print("extended ");
	}

	if (packet->getRtr()) {
		// Remote transmission request, packet contains no data
		Serial.print("RTR ");
	}

	Serial.print("packet with id 0x");
	Serial.print(packet->getId(), HEX);

	// only print packet data for non-RTR packets
	if (packet->getRtr()) {
		Serial.print(" and requested length ");
		Serial.println(packet->getDlc());
	} else {
		Serial.print(" and length ");
		Serial.println(packet->getDlc());

		for (int i = 0; i < packet->getDlc(); i++) {
			Serial.print(packet->getData()[i]);
		}

		Serial.println();
	}

	Serial.println();
}
