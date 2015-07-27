#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.brick_master import Master

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    m = Master(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Get current stack voltage (unit is mV)
    stack_voltage = m.get_stack_voltage()
    print('Stack Voltage: ' + str(stack_voltage/1000.0) + ' V')

    # Get current stack current (unit is mA)
    stack_current = m.get_stack_current()
    print('Stack Current: ' + str(stack_current/1000.0) + ' A')

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
