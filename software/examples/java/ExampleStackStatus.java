import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickMaster;

public class ExampleStackStatus {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XXYYZZ to the UID of your Master Brick
	private static final String UID = "XXYYZZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickMaster master = new BrickMaster(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current stack voltage (unit is mV)
		int stackVoltage = master.getStackVoltage(); // Can throw com.tinkerforge.TimeoutException
		System.out.println("Stack Voltage: " + stackVoltage/1000.0 + " V");

		// Get current stack current (unit is mA)
		int stackCurrent = master.getStackCurrent(); // Can throw com.tinkerforge.TimeoutException
		System.out.println("Stack Current: " + stackCurrent/1000.0 + " A");

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
