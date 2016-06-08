//
// test08
//
// local integer arrays
//
// expected output: 1987654321 (no newline)
//

module test08;

procedure test();
var a : integer[10];
    i: integer;
begin
  a[0] := 1;

  i := 1;
  while (i < 10) do
    a[i] := 10-i;
    i := i+1
  end;

  i := 0;
  while (i < 10) do
    WriteInt(a[i]);
    i := i+1
  end
end test;

begin
  test()
end test08.
