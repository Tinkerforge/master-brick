#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/brick_master'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'a4GeP9ZpQFT' # Change to your UID

ipcon = IPConnection.new HOST, PORT # Create IP connection to brickd
master = BrickMaster.new UID # Create device object
ipcon.add_device master # Add device to IP connection
# Don't use device before it is added to a connection

# Get voltage and current from stack (in mV/mA)
voltage = master.get_stack_voltage
current = master.get_stack_current

puts "Stack Voltage: #{voltage/1000.0} V"
puts "Stack Current: #{current/1000.0} A"

puts 'Press key to exit'
$stdin.gets
ipcon.destroy
