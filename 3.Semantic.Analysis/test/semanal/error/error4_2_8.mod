module number;

procedure f(a : integer; b : integer);
begin
    a := b;
    return
end f;

begin
//    f(); // fail
//    f(3); // fail
//    f(3,4,5); // fail
    f(6,7,8,9); // fail
    f(987654321,123456789); // pass
    return
end number.
