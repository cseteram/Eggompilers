module simple07;

var i : boolean;

function f(a,b : boolean) : boolean;
begin
    return a && b && a && b
end f;

begin
    i := f(true, true) || f(true, false)
end simple07.
