//
// test9
//
// IR generation
//

module test9;

var i: integer;
    b: boolean;
    A: integer[10];
    B: boolean[10];

begin
  b := true;
  b := false;
  b := B[0];
  b := A[0] > 1;

  if (b) then
    b := true
  else
    b := false
  end;

  if (B[0]) then
    b := true
  else
    b := false
  end;

  if (A[0] > 1) then
    b := true
  else
    b := false
  end;

  i := 0
end test9.
