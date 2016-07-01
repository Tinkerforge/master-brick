#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XXYYZZ # Change XXYYZZ to the UID of your Master Brick

# Get current stack voltage (unit is mV)
tinkerforge call master-brick $uid get-stack-voltage

# Get current stack current (unit is mA)
tinkerforge call master-brick $uid get-stack-current
