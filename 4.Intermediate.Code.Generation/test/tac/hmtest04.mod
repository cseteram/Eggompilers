//
// hmtest04
//
// IR generation
//

module hmtest04;

var A: integer[10];

function bar(A: integer[]) : integer;
begin
    return A[0]
end bar;

begin
    bar(A);
    return
end hmtest04.
