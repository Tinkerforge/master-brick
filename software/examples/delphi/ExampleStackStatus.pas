program ExampleStackStatus;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickMaster;

type
  TExample = class
  private
    ipcon: TIPConnection;
    master: TBrickMaster;
  public
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'ap8MjSchCcU'; { Change to your UID }

var
  e: TExample;

procedure TExample.Execute;
var voltage, current: word;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  master := TBrickMaster.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Get voltage and current from stack (in mV/mA) }
  voltage := master.GetStackVoltage;
  current := master.GetStackCurrent;

  WriteLn(Format('Stack Voltage: %f V', [voltage/1000.0]));
  WriteLn(Format('Stack Current: %f A', [current/1000.0]));

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy;
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
