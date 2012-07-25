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
  UID = 'ap8MjSchCcU';

var
  e: TExample;

procedure TExample.Execute;
var voltage, current: word;
begin
  { Create IP connection to brickd }
  ipcon := TIPConnection.Create(HOST, PORT);

  { Create device object }
  master := TBrickMaster.Create(UID);

  { Add device to IP connection }
  ipcon.AddDevice(master);
  { Don't use device before it is added to a connection }

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
