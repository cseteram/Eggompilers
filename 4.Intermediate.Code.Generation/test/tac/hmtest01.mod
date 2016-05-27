//
// test6
//
// IR generation
//

module test6;

var i, j: integer;
    A, B: integer[10][5];

procedure foo1(M: integer[][]);
var a, b, c: integer;
    N: integer[7][8];
begin
    N[a][b] := c;
    if (N[6][5] < M[DIM(M, 1) - 1][DIM(M, 2) - 1]) then
        N[0][0] := N[1][1];
        M[0][0] := N[a][b];
        return
    else
        return
    end;
    return
end foo1;

procedure foo2(M: integer[][]);
var n, m: integer;
    N: integer[1][1][1][1];
begin
    n := M[i][j];
    N[5-5][2/(m - m)][DIM(N, 4) - 1][n] := M[N[0][0][0][0]][M[0][0]];
    return
end foo2;

procedure foo3(M: integer[][]; k: integer);
var n, m: integer;
    N: integer[10][10][10];
    O: integer[1][2];
begin
    bar2(k);
    foo1(M);
    foo2(O);
    N[1+2][M[0][0]][0] := M[i*j][ReadInt()];
  return
end foo3;

procedure bar1(M: integer[][]);
begin
  WriteInt(M[1][2])
end bar1;

procedure bar2(m: integer);
begin
  WriteInt(m)
end bar2;

begin
  bar2(A[0][0]);
  bar1(A);
  foo1(B);
  foo2(B);
  foo3(A, B[1][2]);
  return
end test6.
