<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickMaster.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickMaster;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'a4GeP9ZpQFT'; // Change to your UID

$ipcon = new IPConnection(); // Create IP connection
$master = new BrickMaster(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Get voltage and current from stack (in mV/mA)
$voltage = $master->getStackVoltage();
$current = $master->getStackCurrent();

echo "Stack Voltage: " . $voltage / 1000.0 . " V\n";
echo "Stack Current: " . $current / 1000.0 . " A\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
