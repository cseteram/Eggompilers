//
// char declaration and definitions
//
// does not require array or string support
//

module char03;

var i: integer;

procedure PrintInt(v: integer);
var r: integer;
begin
  r := v - v / 10 * 10;
  v := v / 10;
  if (v > 0) then PrintInt(v) end;
  if (r = 0) then WriteChar('0') end;
  if (r = 1) then WriteChar('1') end;
  if (r = 2) then WriteChar('2') end;
  if (r = 3) then WriteChar('3') end;
  if (r = 4) then WriteChar('4') end;
  if (r = 5) then WriteChar('5') end;
  if (r = 6) then WriteChar('6') end;
  if (r = 7) then WriteChar('7') end;
  if (r = 8) then WriteChar('8') end;
  if (r = 9) then WriteChar('9') end
end PrintInt;

begin
  i := 1;
  while (i # 0) do
    i := ReadInt();
    PrintInt(i); WriteLn()
  end
end char03.
