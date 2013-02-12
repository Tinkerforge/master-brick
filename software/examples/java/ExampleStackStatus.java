import com.tinkerforge.BrickMaster;
import com.tinkerforge.IPConnection;

public class ExampleStackStatus {
	private static final String host = "localhost";
	private static final int port = 4223;
	private static final String UID = "apaYPbNgTw3"; // Change to your UID

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions you
	//       might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickMaster master = new BrickMaster(UID, ipcon); // Create device object

		ipcon.connect(host, port); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get voltage and current from stack (in mV/mA)
		int voltage = master.getStackVoltage();
		int current = master.getStackCurrent();

		System.out.println("Stack Voltage: " + voltage/1000.0 + " V");
		System.out.println("Stack Current: " + current/1000.0 + " A");

		System.console().readLine("Press key to exit\n");
		ipcon.disconnect();
	}
}
