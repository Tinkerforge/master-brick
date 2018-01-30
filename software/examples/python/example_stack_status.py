#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XXYYZZ" # Change XXYYZZ to the UID of your Master Brick

from tinkerforge.ip_connection import IPConnection
from tinkerforge.brick_master import BrickMaster

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    master = BrickMaster(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Get current stack voltage
    stack_voltage = master.get_stack_voltage()
    print("Stack Voltage: " + str(stack_voltage/1000.0) + " V")

    # Get current stack current
    stack_current = master.get_stack_current()
    print("Stack Current: " + str(stack_current/1000.0) + " A")

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
