//
// test02
//
// computations with integers arrays
// also test alignment of global data
//
// expected output: 1987654321 (no newline)
//

module test02;

var a : integer[10];
    b : boolean;
    c : boolean[2];
    i : integer;

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
end test02.
