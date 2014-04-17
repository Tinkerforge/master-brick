function octave_example_stack_status
    more off;
    
    HOST = "localhost";
    PORT = 4223;
    UID = "68WRjG"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    master= java_new("com.tinkerforge.BrickMaster", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get voltage and current from stack (in mV/mA)
    voltage = master.getStackVoltage();
    current = master.getStackCurrent();
    fprintf("Stack Voltage: %g V\n", voltage/1000.0);
    fprintf("Stack Current: %g A\n", current/1000.0);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end
