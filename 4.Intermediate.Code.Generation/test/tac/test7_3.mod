//
// test7
//
// IR generation
//

module test7_3;

var i: integer;

begin
  if ((i < 3) && (i > 0)) then
    i := 0;
    if ((i > 9) || (i > 7)) then
        i := 987654321
    else
        i := -i;
        if (((i > 1111) && (i < 9999)) || (i = 5000)) then
            i := 1029384756
        else
            i := -i + 1
        end
    end;
    i := 0
  end
end test7_3.
