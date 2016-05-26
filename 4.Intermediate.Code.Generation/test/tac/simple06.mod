module simple06;

var i : boolean;

function f : boolean;
begin
    return true
end f;

begin
    if (f()) then
        i := f()
    end;
 
    i := (f() && f())
end simple06.
