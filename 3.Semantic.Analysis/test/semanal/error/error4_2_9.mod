module number;

procedure f(a : integer; b : integer);
begin
    a := b;
    return
end f;

begin
    f(5, true); // fail
    f('a', 4); // fail
    f("hello", "world\n"); // fail

    f(3,5); // pass
    return
end number.
