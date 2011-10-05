using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "apaYPwpooEw"; // Change to your UID

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(HOST, PORT); // Create connection to brickd
		BrickMaster master = new BrickMaster(UID); // Create device object
		ipcon.AddDevice(master); // Add device to ip connection
		// Don't use device before it is added to a connection


		// Get voltage and current from stack (in mV/mA)
		ushort voltage;
		ushort current;
		master.GetStackVoltage(out voltage);
		master.GetStackCurrent(out current);

		System.Console.WriteLine("Stack Voltage: " + voltage/1000.0 + " V");
		System.Console.WriteLine("Stack Current: " + current/1000.0 + " A");

		System.Console.WriteLine("Press ctrl+c to exit");
		ipcon.JoinThread();
    }
}
