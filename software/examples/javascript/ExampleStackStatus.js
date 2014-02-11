var IPConnection = require('Tinkerforge/IPConnection');
var BrickMaster= require('Tinkerforge/BrickMaster');

var HOST = 'localhost';
var PORT = 4223;
var UID = '6CPWYS';// Change to your UID

var ipcon = new IPConnection();// Create IP connection
var master = new BrickMaster(UID, ipcon);// Create device object

ipcon.connect(HOST, PORT,
    function(error) {
        if(error === IPConnection.ERROR_ALREADY_CONNECTED) {
            console.log('Error: Already connected');        
        }
    }
);//Connect to brickd

// Don't use device before ipcon is connected
ipcon.on(IPConnection.CALLBACK_CONNECTED,
    function(connectReason) {
        // Get voltage and current from stack (in mV/mA)
        master.getStackVoltage(
            function(voltage) {
                console.log('Stack Voltage: '+voltage/1000+' V');
            },
            function(error) {
                if(error === IPConnection.RESPONSE_TIMED_OUT) {
                  console.log('Error: The request timed out');
                }
            }
        );
        master.getStackCurrent(
            function(current) {
                console.log('Stack Current: '+current/1000+' A');
            },
            function(error) {
                if(error === IPConnection.RESPONSE_TIMED_OUT) {
                  console.log('Error: The request timed out');
                }
            }
        );
    }
);

console.log("Press any key to exit ...");
process.stdin.on('data', function(data) {
	    ipcon.disconnect(
            function(error) {
                if(error === IPConnection.ERROR_NOT_CONNECTED) {
                    console.log('Error: Not connected');        
                }
            }
        );
process.exit(0);
});

