#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "aySDPZAhvvd" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.brick_master import Master

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    master = Master(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Get voltage and current from stack (in mV/mA)
    voltage = master.get_stack_voltage()
    current = master.get_stack_current()

    print('Stack Voltage: ' + str(voltage/1000.0) + ' V')
    print('Stack Current: ' + str(current/1000.0) + ' A')

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
