module array;

var a,b,c: integer[5][5];

procedure add(A,B,C: integer[5][5]);
var i,j: integer;
begin
  i := 0;
  while (i < 5) do
    j := 0;
    while (j < 5) do
      C[i][j] := A[i][j] + B[i][j]
    end
  end
end add;

procedure addB(A,B,C: integer[][]);
var i,j: integer;
begin
  i := 0;
  while (i < 5) do
    j := 0;
    while (j < 5) do
      C[i][j] := A[i][j] + B[i][j]
    end
  end
end addB;

begin
  add(a,b,c)
end array.
