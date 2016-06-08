//
// test06
//
// computations with boolean arrays
//
// expected output: 0001111111 (no newline)
//

module test06;

var a : boolean[10];

procedure test(a: boolean[]);
var i: integer;
begin
  i := 0;
  while (i < 10) do
    a[i] := i > 2;
    i := i+1
  end;

  i := 0;
  while (i < 10) do
    if (a[i]) then WriteInt(1)
    else WriteInt(0)
    end;
   i := i+1
  end
end test;

begin
  test(a)
end test06.
