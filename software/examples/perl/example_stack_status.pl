#!/usr/bin/perl  

use Tinkerforge::IPConnection;
use Tinkerforge::BrickMaster;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => '6CPWYS'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $master = Tinkerforge::BrickMaster->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get voltage and current from stack (in mV/mA)
my $voltage = $master->get_stack_voltage()/1000.0;
print "Stack Voltage: $voltage V\n";

my $current = $master->get_stack_current()/1000.0;
print "Stack Current: $current A\n";

print "Press any key to exit...\n";
<STDIN>;
$ipcon->disconnect();
