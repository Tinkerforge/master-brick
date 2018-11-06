use std::{error::Error, io};

use tinkerforge::{ipconnection::IpConnection, master_brick::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XXYYZZ"; // Change XXYYZZ to the UID of your Master Brick

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let master_brick = MasterBrick::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Get current stack voltage
    let stack_voltage = master_brick.get_stack_voltage().recv()?;
    println!("Stack Voltage: {}{}", stack_voltage as f32 / 1000.0, " V");

    // Get current stack current
    let stack_current = master_brick.get_stack_current().recv()?;
    println!("Stack Current: {}{}", stack_current as f32 / 1000.0, " A");

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
