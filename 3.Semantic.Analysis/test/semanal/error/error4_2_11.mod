module invalid;

var a : integer;

begin
    a := -2147483648; // pass
    // a := +2147483648; // fail
    a := -2147483648 + 3; // pass
    a := -2147483648 * 3; // fail
    return
end invalid.
