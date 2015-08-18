Imports Tinkerforge

Module ExampleStackStatus
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim master As New BrickMaster(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get current stack voltage (unit is mV)
        Dim stackVoltage As Integer = master.GetStackVoltage()
        System.Console.WriteLine("Stack Voltage: " + (stackVoltage/1000.0).ToString() + " V")

        ' Get current stack current (unit is mA)
        Dim stackCurrent As Integer = master.GetStackCurrent()
        System.Console.WriteLine("Stack Current: " + (stackCurrent/1000.0).ToString() + " A")

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
