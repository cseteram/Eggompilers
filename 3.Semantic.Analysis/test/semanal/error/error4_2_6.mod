module bin;

var a : integer;
    b : boolean;
    c : char;
    arr : integer[5];

procedure compare(ptr : integer[5]; ptr2 : integer[5]);
var s : char[14];

begin
    a := a + b; // fail
    a := a + c; // fail
    a := a + arr; // fail
    a := a + ptr; // fail

    a := arr - a; 
    arr := arr - a; // fail
    arr := arr - b; // fail
    arr := arr - c; // fail
    arr := ptr2 - ptr; // fail

    ptr := ptr / arr; // fail

    s := s + "Hello, World\n"; // fail

    ptr := ptr2; // pass

    return
end compare;

begin

end bin.
