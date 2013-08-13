#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# get voltage and current from stack (in mV/mA)
tinkerforge call master-brick $uid get-stack-voltage
tinkerforge call master-brick $uid get-stack-current
