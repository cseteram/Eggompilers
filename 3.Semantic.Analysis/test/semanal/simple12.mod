module simple12;

procedure f();
begin
    // WriteStr("Hello, World\n")
end f;

function _str_1 : integer;
begin
    WriteStr("Hello, World\n");
    WriteStr("Hello, World\n");
    WriteStr("Hello, World\n");
    return 1
end _str_1;

begin
    f();
    WriteInt(_str_1())
end simple12.
