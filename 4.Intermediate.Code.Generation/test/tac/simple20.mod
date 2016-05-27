module simple20;

var b : boolean;

function f() : boolean;
begin
end f;

function g() : boolean;
begin
end g;

function h() : boolean;
begin
end h;

begin

    b := f() && g() || h() && g() || !h();
    if (f()) then else end

end simple20.
