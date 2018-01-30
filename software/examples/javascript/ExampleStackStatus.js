var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XXYYZZ'; // Change XXYYZZ to the UID of your Master Brick

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var master = new Tinkerforge.BrickMaster(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Get current stack voltage
        master.getStackVoltage(
            function (stackVoltage) {
                console.log('Stack Voltage: ' + stackVoltage/1000.0 + ' V');
            },
            function (error) {
                console.log('Error: ' + error);
            }
        );

        // Get current stack current
        master.getStackCurrent(
            function (stackCurrent) {
                console.log('Stack Current: ' + stackCurrent/1000.0 + ' A');
            },
            function (error) {
                console.log('Error: ' + error);
            }
        );
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
