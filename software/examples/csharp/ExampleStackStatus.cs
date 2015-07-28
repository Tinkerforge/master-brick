using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickMaster master = new BrickMaster(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current stack voltage (unit is mV)
		int stackVoltage = master.GetStackVoltage();
		System.Console.WriteLine("Stack Voltage: " + stackVoltage/1000.0 + " V");

		// Get current stack current (unit is mA)
		int stackCurrent = master.GetStackCurrent();
		System.Console.WriteLine("Stack Current: " + stackCurrent/1000.0 + " A");

		System.Console.WriteLine("Press enter to exit");
		System.Console.ReadLine();
		ipcon.Disconnect();
	}
}
