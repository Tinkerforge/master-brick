var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = '6CPWYS';// Change to your UID

var ipcon = new Tinkerforge.IPConnection();// Create IP connection
var master = new Tinkerforge.BrickMaster(UID, ipcon);// Create device object

ipcon.connect(HOST, PORT,
    function(error) {
        console.log('Error: '+error);        
    }
);// Connect to brickd

// Don't use device before ipcon is connected
ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function(connectReason) {
        // Get voltage and current from stack (in mV/mA)
        master.getStackVoltage(
            function(voltage) {
                console.log('Stack Voltage: '+voltage/1000+' V');
            },
            function(error) {
                console.log('Error: '+error);
            }
        );
        master.getStackCurrent(
            function(current) {
                console.log('Stack Current: '+current/1000+' A');
            },
            function(error) {
                console.log('Error: '+error);
            }
        );
    }
);

console.log("Press any key to exit ...");
process.stdin.on('data',
    function(data) {
        ipcon.disconnect();
        process.exit(0);
    }
);

