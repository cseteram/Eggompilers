// pstest01.mod
// function declaration without return statements
// remark that it is undefined behaviour in c and c++, not compile error

module pstest01;

var a : integer;

function f(a : integer) : integer;

begin
    a := 3;
    WriteInt(a)
end f;

begin
    a := ReadInt();
    a := f(a);
    WriteInt(a)
end pstest01.
