//
// test8
//
// IR generation
//

module test8;

var i: integer;
    b: boolean;

begin
  b := (i < 3) && (i > 0);
  if (b) then
    i := 0
  end
end test8.
