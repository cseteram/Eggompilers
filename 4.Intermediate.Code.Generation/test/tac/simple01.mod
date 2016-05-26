module simple00;

var sum : integer;

function f(a, b : integer) : integer;
begin
    return a + b
end f;

begin
    sum := f(1, 2);
    WriteInt(sum)
end simple00.
