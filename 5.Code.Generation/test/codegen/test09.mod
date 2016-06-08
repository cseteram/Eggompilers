//
// test09
//
// local boolean arrays
//
// expected output: 0001111111 (no newline)
//

module test09;

procedure test();
var a : boolean[10];
    i: integer;
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
  test()
end test09.
