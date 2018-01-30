Imports System
Imports Tinkerforge

Module ExampleStackStatus
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XXYYZZ" ' Change XXYYZZ to the UID of your Master Brick

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim master As New BrickMaster(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get current stack voltage
        Dim stackVoltage As Integer = master.GetStackVoltage()
        Console.WriteLine("Stack Voltage: " + (stackVoltage/1000.0).ToString() + " V")

        ' Get current stack current
        Dim stackCurrent As Integer = master.GetStackCurrent()
        Console.WriteLine("Stack Current: " + (stackCurrent/1000.0).ToString() + " A")

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
