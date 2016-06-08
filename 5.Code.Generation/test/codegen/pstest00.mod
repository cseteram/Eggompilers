// pstest00.mod
// this tests the function with boolean return type,
// integer array, and boolean array.

module pstest00;

var a : integer[111111];
    b : integer[1111][1111];
    n, m, sum : integer;

procedure input;
var i, j : integer;
begin
    n := ReadInt(); m := ReadInt();
    i := 0; j := 0;

    while (i < n*m) do 
        a[i] := ReadInt();
        i := i+1
    end;
    
    i := 0;
    while (i < n) do
        while (j < m) do
            b[i][j] := ReadInt();
            j := j+1
        end;
        i := i+1
    end;

    return
end input;

function div(value : integer ; q : integer) : integer;
begin
    return value / q
end div;

function mod(value : integer ; q : integer) : integer;
begin
    return value - value / q * q
end mod;

function pos(index : integer ; arr : integer[][]) : boolean;
var x : integer;
begin
    x := mod(index, n * m);
    if (arr[div(x,m)][mod(x,m)] = 0) then
        return false
    end;
    return true
end pos;

procedure process();
var i : integer;
begin
    i := 0;

    while (pos(a[i], b) && (i < n*m)) do
        sum := sum + a[i];
        i := i+1
    end

end process;

procedure output();
begin
    WriteInt(sum)
end output;

begin
    input();
    process();
    output()
end pstest00.
