module assign;

var a : integer;
    b : boolean;
    c : integer[3][4];
    d : integer[5];

begin
    a := b; // fail
    c := a; // fail
    c := d; // fail
    c[0] := d; // fail
    d[0] := c[0]; // fail
    d[0] := c[0][0]; // pass
    return
end assign.
