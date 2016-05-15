module simple06;
var a : integer;
begin
    a := 18446744073709551616987654321; // fail
    a := 4294967296; // fail
    a := 2147483647 // pass

end simple06.
