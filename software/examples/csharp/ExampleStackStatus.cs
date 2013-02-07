using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "apaYPwpooEw"; // Change to your UID

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickMaster master = new BrickMaster(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get voltage and current from stack (in mV/mA)
		int voltage = master.GetStackVoltage();
		int current = master.GetStackCurrent();

		System.Console.WriteLine("Stack Voltage: " + voltage/1000.0 + " V");
		System.Console.WriteLine("Stack Current: " + current/1000.0 + " A");

		System.Console.WriteLine("Press key to exit");
		System.Console.ReadKey();
		ipcon.Disconnect();
	}
}
