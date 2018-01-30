function octave_example_stack_status()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XXYYZZ"; % Change XXYYZZ to the UID of your Master Brick

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    master = javaObject("com.tinkerforge.BrickMaster", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current stack voltage
    stackVoltage = master.getStackVoltage();
    fprintf("Stack Voltage: %g V\n", stackVoltage/1000.0);

    % Get current stack current
    stackCurrent = master.getStackCurrent();
    fprintf("Stack Current: %g A\n", stackCurrent/1000.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
