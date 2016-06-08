module instr02;

var ret : integer;

procedure f(i : integer; j : boolean; k : char);
begin
end f;

function g(i : integer; j : boolean; k : char) : integer;
begin
    return -1
end g;

begin
    // function-realted instructions
    f(0, true, 'a');
    g(0, true, 'a');
    ret := g(1, false, 'b');
    return
end instr02.
