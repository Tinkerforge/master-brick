import com.tinkerforge.BrickMaster;
import com.tinkerforge.IPConnection;

public class ExampleStackStatus {
	private static final String host = new String("localhost");
	private static final int port = 4223;
	private static final String UID = new String("apaYPbNgTw3"); // Change to your UID
	
	// Note: To make the example code cleaner we do not handle exceptions. Exceptions you
	//       might normally want to catch are described in the commnents below
	public static void main(String args[]) throws Exception {
		// Create connection to brickd
		IPConnection ipcon = new IPConnection(host, port); // Can throw IOException
		BrickMaster master = new BrickMaster(UID); // Create device object

		// Add device to IP connection
		ipcon.addDevice(master); // Can throw IPConnection.TimeoutException
		// Don't use device before it is added to a connection

		// Get voltage and current from stack (in mV/mA)
		int voltage = master.getStackVoltage();
		int current = master.getStackCurrent();

		System.out.println("Stack Voltage: " + voltage/1000.0 + " V");
		System.out.println("Stack Current: " + current/1000.0 + " A");

		System.out.println("Press ctrl+c to exit");
		ipcon.joinThread();
	}
}
