Imports Tinkerforge

Module ExampleStackStatus
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "6kpiwr" ' Change to your UID

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim master As New BrickMaster(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get voltage and current from stack (in mV/mA)
        Dim voltage As Integer = master.GetStackVoltage()
        Dim current As Integer = master.GetStackCurrent()

        System.Console.WriteLine("Stack Voltage: " + (voltage/1000.0).ToString() + " V")
        System.Console.WriteLine("Stack Current: " + (current/1000.0).ToString() + " A")

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
