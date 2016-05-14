// pstest00.mod
// this tests the function with boolean return type,
// integer array, and boolean array.

module pstest00;

var a : integer[111111];
    b : integer[1111][1111];
    sum : integer;

procedure input;
begin

end input;

function pos(index : integer ; arr : integer[][]) : boolean;
begin
    // TODO
end pos;

function div(value : integer ; q : integer) : integer;
begin
    return value / q
end div;

function mod(value : integer ; q : integer) : integer;
begin
    return value - value / q * q
end mod;

procedure process();
begin
    // TODO
end process;

procedure output();
begin
    // TODO
end output;

begin
    input();
    process();
    output()
end pstest00.
