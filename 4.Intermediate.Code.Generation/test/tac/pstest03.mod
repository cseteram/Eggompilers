// pstest03.mod
// tests function with array return type

module pstest03;

var a : integer[3][5][4];

function f(a : integer[][5][]) : integer;
begin
  if (a[3][2][1] > 0) then 
    return a
  end
end f;

begin
    WriteInt(f(a))
end pstest03.
