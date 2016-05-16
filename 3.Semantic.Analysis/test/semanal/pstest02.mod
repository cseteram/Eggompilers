// pstest02.mod
// statSequence = [ statement { ";" statement } ]
// therefore, statSequence can be empty.

module pstest02;

var a : boolean;

function input() : boolean;

var x : integer;

begin
    x := ReadInt();
    if (x = 0) then 
        return false
    end;
    return true
end input;

procedure empty;
begin
end empty;

begin
    WriteStr ("module begin");
    WriteStr ("enter a integer. nonzero means true, and zero means false");
    a := input();
    empty();

    if (a) then end;
    if (!a) then end;
    WriteStr("if statements end");

    while (!a) do end;
    WriteStr("while statements end");

    WriteStr("module end")
end pstest02.

