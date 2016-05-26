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
    N: integer[10][10][10];
begin
  N[1+2][M[0][0]][0] := M[i*j][ReadInt()]
end foo;

procedure bar(M: integer[][]);
begin
  WriteInt(M[1][2])
end bar;

begin
  i := 0;
  foo(A)
end test6.
