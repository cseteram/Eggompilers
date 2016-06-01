module simple09;

var i : integer;
    j : boolean;
    b1, b2 : boolean;
function f(i : integer) : boolean;
begin
    return true
end f;

procedure g(b : boolean);
begin

end g;

begin
    i := +i;
    i := -i;
    j := !j && j;
    j := j && !j;
    j := !j && !j;
    j := !(j && j);
    g(!(true || false));

    j := !(b1 && b2);
    j := !j;
    j := !f(1);
    j := (-i > -i);
    i := -2147483648;

    return
end simple09.
