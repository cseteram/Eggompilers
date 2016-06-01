//
// test6
//
// IR generation
//

module test6;

var i: integer;
    A: integer[10][5];

procedure foo(M: integer[][]);
begin
  M[1][3] := i
end foo;

begin
  i := 0;
  foo(A)
end test6.
