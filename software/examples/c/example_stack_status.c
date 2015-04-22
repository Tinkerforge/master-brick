#include <stdio.h>

#include "ip_connection.h"
#include "brick_master.h"

#define HOST "localhost"
#define PORT 4223
#define UID "aySDPZAhvvd" // Change to your UID

int main() {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	Master master;
	master_create(&master, UID, &ipcon); 

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		exit(1);
	}
	// Don't use device before ipcon is connected

	// Get voltage and current from stack (in mV/mA)
	uint16_t voltage; 
	master_get_stack_voltage(&master, &voltage);
	uint16_t current;
	master_get_stack_current(&master, &current);

	printf("Stack Voltage: %f V\n", voltage/1000.0);
	printf("Stack Current: %f A\n", current/1000.0);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
