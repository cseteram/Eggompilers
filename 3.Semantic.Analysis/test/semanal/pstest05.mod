// pstest05.mod
// tests for NULL type expression

module pstest05;

procedure f;
begin
    return
end f;

procedure g;
begin
//    return f()
end g;

function f2 : integer;
begin
    return f()
end f2;

begin
//    return f(g())
end pstest05.
