use std::{io, error::Error};

use tinkerforge::{ip_connection::IpConnection, 
                  master_brick::*};


const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XXYYZZ"; // Change XXYYZZ to the UID of your Master Brick.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let master = MasterBrick::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
    // Don't use device before ipcon is connected.

		// Get current stack voltage.
let stack_voltage = master.get_stack_voltage().recv()?;
		println!("Stack Voltage: {} V", stack_voltage as f32 /1000.0);

		// Get current stack current.
let stack_current = master.get_stack_current().recv()?;
		println!("Stack Current: {} A", stack_current as f32 /1000.0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
