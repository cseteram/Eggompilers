//
// test04
//
// parser test:
// - global integer variables
// - module body
// - integer expressions
//

module test04;

var a,b,c : integer;

begin
  a := 1;
  b := 2;
  c := 3;

  a := a + b + c;
  b := a - b - c;
  c := a + b * c;
  a := -a;
  b := (a + b) / c;
  c := -(-1 + a)
end test04.
