module simple00;

var a : integer[5][5];

procedure f(a: integer[][]; b,c: integer);
var i,j : integer;
begin
  i := b; j := c;
  a[2][2] := i+j
end f;

begin
  f(a,3,5);
  WriteInt(a[2][2]);
  return
end simple00.
