module unary;

var a : integer;
    b : boolean;

begin
    a := +a; // pass
    a := -a; // pass
    b := !b; // pass

    b := +b; // fail
    b := -b; // fail
    a := !a; // fail

    return

end unary.
