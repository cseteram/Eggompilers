//
// hmtest02
//
// IR generation
//

module hmtest02;

var i: integer;
    b: boolean;
    A: integer[10];
    B: boolean[10];

begin
  if (B[0]) then
    b := !B[0]
  else
    b := B[0] || true
  end;

  if (A[0] > i) then
    b := A[0] < i
  else
    b := i < A[0]
  end;
  return
end hmtest02.
