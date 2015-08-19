#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickMaster;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $master = Tinkerforge::BrickMaster->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get current stack voltage (unit is mV)
my $stack_voltage = $master->get_stack_voltage();
print "Stack Voltage: " . $stack_voltage/1000.0 . " V\n";

# Get current stack current (unit is mA)
my $stack_current = $master->get_stack_current();
print "Stack Current: " . $stack_current/1000.0 . " A\n";

print "Press any key to exit...\n";
<STDIN>;
$ipcon->disconnect();
