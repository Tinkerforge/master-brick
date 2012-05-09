<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickMaster.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickMaster;

$host = 'localhost';
$port = 4223;
$uid = 'a4GeP9ZpQFT'; // Change to your UID

$ipcon = new IPConnection($host, $port); // Create IP connection to brickd
$master = new BrickMaster($uid); // Create device object

$ipcon->addDevice($master); // Add device to IP connection
// Don't use device before it is added to a connection

// Get voltage and current from stack (in mV/mA)
$voltage = $master->getStackVoltage();
$current = $master->getStackCurrent();

echo "Stack Voltage: " . $voltage / 1000.0 . " V\n";
echo "Stack Current: " . $current / 1000.0 . " A\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->destroy();

?>
