// re-declaration function

module super_fantasy_war;

var f : integer;

function f(a,b : integer) : integer;
begin
    return a+b
end f;

begin
    WriteInt(f(1,2))
end super_fantasy_war.
