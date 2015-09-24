#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/brick_master'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XXYYZZ' # Change to your UID

ipcon = IPConnection.new # Create IP connection
master = BrickMaster.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get current stack voltage (unit is mV)
stack_voltage = master.get_stack_voltage
puts "Stack Voltage: #{stack_voltage/1000.0} V"

# Get current stack current (unit is mA)
stack_current = master.get_stack_current
puts "Stack Current: #{stack_current/1000.0} A"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
