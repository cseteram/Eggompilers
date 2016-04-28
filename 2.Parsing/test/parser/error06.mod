// re-definition error

module pstest07;

var a,b,c : integer;
    d,e,f : integer;

procedure foo(a : integer; a : integer);
begin
end foo;

begin
    WriteStr("Goodbye, World!\n");
end pstest07.
