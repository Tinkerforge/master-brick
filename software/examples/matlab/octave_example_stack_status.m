function octave_example_stack_status()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XXYYZZ"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    master = java_new("com.tinkerforge.BrickMaster", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current stack voltage (unit is mV)
    stackVoltage = master.getStackVoltage();
    fprintf("Stack Voltage: %g V\n", stackVoltage/1000.0);

    % Get current stack current (unit is mA)
    stackCurrent = master.getStackCurrent();
    fprintf("Stack Current: %g A\n", stackCurrent/1000.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
