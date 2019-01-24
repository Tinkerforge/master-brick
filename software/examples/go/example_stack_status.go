package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
	"github.com/Tinkerforge/go-api-bindings/master_brick"
)

const ADDR string = "localhost:4223"
const UID string = "XXYYZZ" // Change XXYYZZ to the UID of your Master Brick.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	master, _ := master_brick.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Get current stack voltage.
	stackVoltage, _ := master.GetStackVoltage()
	fmt.Printf("Stack Voltage: %f V\n", float64(stackVoltage)/1000.0)

	// Get current stack current.
	stackCurrent, _ := master.GetStackCurrent()
	fmt.Printf("Stack Current: %f A\n", float64(stackCurrent)/1000.0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
