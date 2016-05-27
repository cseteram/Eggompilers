// pstest04.mod
// tests not matchable array parameter

module pstest04;

var a : integer[3][5][5][6];

function f(a : integer[][5][][]) : integer;
begin
    if (a[0][0][0][0] > a[1][1][1][1]) then
//        return a[0][0][0][0]
    else
//        return a[1][1][1][1]
    end;
    return -1
end f;

begin
    WriteInt(f(a))
end pstest04.
