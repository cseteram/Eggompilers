//
// array declaration and definitions
//
// does not require string support
//

module array05;

var A: integer[3][3];
    i: integer;

procedure Set(P: integer[][]; v: integer);
begin
  P[v][v] := 1
end Set;

procedure Print(A: integer[][]);
var i,j,N,M: integer;
begin
  N := DIM(A, 1);
  M := DIM(A, 2);
  i := 0;
  while (i < N) do
    j := 0;
    while (j < M) do
      WriteInt(A[i][j]); WriteLn();
      j := j+1
    end;
    i := i+1
  end
end Print;

procedure Init(a: integer[][]);
var i,j,N,M,c: integer;
begin
  N := DIM(a, 1);
  M := DIM(a, 2);
  c := 0;
  i := 0;
  while (i < N) do
    j := 0;
    while (j < M) do
      a[i][j] := c;
      c := c + 1;
      j := j+1
    end;
    i := i+1
  end;
  Print(a)
end Init;

begin
  WriteInt(11111111); WriteLn();
  Print(A);
  WriteInt(22222222); WriteLn();
  Init(A)
end array05.
