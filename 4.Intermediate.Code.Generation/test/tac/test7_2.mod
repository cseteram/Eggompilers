//
// test7
//
// IR generation
//

module test7_2;

var i: integer;

begin
  if (i < 3) then
    i := 0;
    if (i > 9) then
        i := 987654321
    else
        i := 123456789;
        if (i > 11) then
            i := 1029384756
        else
            i := 0192837465
        end
    end;
    i := 0
  end
end test7_2.
