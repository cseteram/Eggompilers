module simple01;

var a : integer;
    b : boolean;
    c : char;
    arr : integer[5];

begin
  a := b; // fail
  b := c; // fail
  c := a; // fail
  // arr['a'] := a;
  return
end simple01.
