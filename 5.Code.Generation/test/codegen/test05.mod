//
// test05
//
// computations with integers arrays
//
// expected output: 1987654321 (no newline)
//

module test05;

var a : integer[10];

procedure test(a: integer[]);
var i: integer;
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
  test(a)
end test05.
