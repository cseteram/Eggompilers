//
// test6
//
// IR generation
//

module test6;

var i, j: integer;
    A: integer[10][5];

procedure foo(M: integer[][]);
var n, m: integer;
    N: integer[10][5];
begin
  n := 3;
  j := 1111111;
  M[1][n] := i;
  j := 1111111;
  m := M[1][n];
  j := 1111111;
  N[2-1][1+2] := i
end foo;

begin
  i := 0;
  foo(A)
end test6.
