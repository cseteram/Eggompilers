// add, minus, mul, div, equal, A+B problem

module pstest02;

var a,b,c,d,e,f : integer; g : boolean;

function add(a:integer; b:integer) : integer;
begin
    return a + b
end add;

function minus(a:integer; b:integer) : integer;
begin
    return a - b
end minus;

function mul(a:integer; b:integer) : integer;
begin
    return a * b
end mul;

function div(a:integer; b:integer) : integer;
begin
    return a / b
end div;

function equal(a:integer; b:integer) : boolean;
begin
    return a = b
end equal;

procedure printAdd(a:integer; b:integer);
begin
    WriteInt(a+b)
end printAdd;

begin
    a := 1; b := 2;
    c := add(a,b);
    d := minus(a,b);
    e := mul(a,b);
    f := div(a,b);
    g := equal(a,b);
    printAdd(a,b)
end pstest02.
