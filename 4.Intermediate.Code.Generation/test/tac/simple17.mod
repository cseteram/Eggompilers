module simple17;

var i : integer;

begin

//    i := 2147483648 // error
    i := +9999;
    i := +(+9999);
    i := +(+(+(+(+9999))));
    i := -2147483648;
    i := -(-2147483648);
    i := -(-(-(-(-2147483648))));
    i := -2147483648 + 3 + (-7) - (-8);
//    i := -2147483648 * 3; // error when simple
    return

end simple17.
