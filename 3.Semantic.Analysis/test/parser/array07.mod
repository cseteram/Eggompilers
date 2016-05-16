//
// array declaration and definitions
//
// does not require string support
//

module array07;

var sum: integer[5][5];

procedure Print(a: integer[][]);
var i,j: integer;
    N,M: integer;
begin
  N := DIM(a, 1);
  M := DIM(a, 2);

  i := 0;
  while (i < N) do
    j := 0;
    while (j < M) do
      WriteInt(a[i][j]);
      j := j+1
    end;
    i := i+1
  end
end Print;

procedure Init(a: integer[][]);
var i,j,c: integer;
    N,M  : integer;
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
  end
end Init;

procedure Add(s1,s2,d: integer[][]);
var i,j,c: integer;
    N,M  : integer;
begin
  N := DIM(d, 1);
  M := DIM(d, 2);

  c := 0;
  i := 0;
  while (i < N) do
    j := 0;
    while (j < M) do
      d[i][j] := s1[i][j] + s2[i][j];
      c := c + 1;
      j := j+1
    end;
    i := i+1
  end
end Add;

procedure Test;
var a,b: integer[5][5];
    c: integer[5];
begin
  Init(a);
  Init(b);
  Add(a,b,sum)
end Test;

begin
  Test();
  Print(sum)
end array07.
