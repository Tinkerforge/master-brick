#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/brick_master'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'a4GeP9ZpQFT' # Change to your UID

ipcon = IPConnection.new # Create IP connection
master = BrickMaster.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get voltage and current from stack (in mV/mA)
voltage = master.get_stack_voltage
current = master.get_stack_current

puts "Stack Voltage: #{voltage/1000.0} V"
puts "Stack Current: #{current/1000.0} A"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
