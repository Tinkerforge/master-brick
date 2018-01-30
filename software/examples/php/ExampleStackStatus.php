<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickMaster.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickMaster;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XXYYZZ'; // Change XXYYZZ to the UID of your Master Brick

$ipcon = new IPConnection(); // Create IP connection
$master = new BrickMaster(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Get current stack voltage
$stack_voltage = $master->getStackVoltage();
echo "Stack Voltage: " . $stack_voltage/1000.0 . " V\n";

// Get current stack current
$stack_current = $master->getStackCurrent();
echo "Stack Current: " . $stack_current/1000.0 . " A\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
