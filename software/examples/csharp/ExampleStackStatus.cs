using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XXYYZZ"; // Change XXYYZZ to the UID of your Master Brick

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickMaster master = new BrickMaster(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current stack voltage
		int stackVoltage = master.GetStackVoltage();
		Console.WriteLine("Stack Voltage: " + stackVoltage/1000.0 + " V");

		// Get current stack current
		int stackCurrent = master.GetStackCurrent();
		Console.WriteLine("Stack Current: " + stackCurrent/1000.0 + " A");

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
