//
// array declaration and definitions
//
// does not require string support
//

module array04;

var A: integer[3][3];
    i: integer;

procedure Set(P: integer[][]; v: integer);
begin
  P[v][v] := 1
end Set;

procedure print;
var i,j: integer;
begin
  i := 0;
  while (i < 3) do
    j := 0;
    while (j < 3) do
      WriteInt(A[i][j]);
      j := j+1
    end;
    i := i+1
  end;
  WriteLn()
end print;

begin
  WriteInt(11111111); WriteLn();
  print();
  A[0][0] := 2;
  i := A[0][0];
  Set(A, i);
  WriteInt(22222222); WriteLn();
  print()
end array04.
