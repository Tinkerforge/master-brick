#include <stdio.h>

#include "ip_connection.h"
#include "brick_master.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XXYYZZ" // Change XXYYZZ to the UID of your Master Brick

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	Master master;
	master_create(&master, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Get current stack voltage
	uint16_t stack_voltage;
	if(master_get_stack_voltage(&master, &stack_voltage) < 0) {
		fprintf(stderr, "Could not get stack voltage, probably timeout\n");
		return 1;
	}

	printf("Stack Voltage: %f V\n", stack_voltage/1000.0);

	// Get current stack current
	uint16_t stack_current;
	if(master_get_stack_current(&master, &stack_current) < 0) {
		fprintf(stderr, "Could not get stack current, probably timeout\n");
		return 1;
	}

	printf("Stack Current: %f A\n", stack_current/1000.0);

	printf("Press key to exit\n");
	getchar();
	master_destroy(&master);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
