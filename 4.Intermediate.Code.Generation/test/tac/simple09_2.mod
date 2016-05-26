module simple09_2;

var b1,b2,b3,b4,b5 : boolean;
    ret : boolean;

function f(arg : boolean) : boolean;
begin
    ret := !(b1 && b2);
    ret := !true;
    ret := f(!(b1 && b2));
    f(!(b1 && b2));
    if (!(b1 && b2)) then end;
    while (!(b1 && b2)) do end;
    return (!(b1 && b2))
end f;

begin

end simple09_2.
