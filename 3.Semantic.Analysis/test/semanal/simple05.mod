module simple05;

var a : integer;

begin
  
  a := -2147483647; // pass
  a := 2147483647; // pass
  a := +2147483647; // pass
  
  a := -2147483648; // pass
  a := 2147483648; // fail
  a := +2147483648; // fail
  
  a := -2147483649 // fail
  a := 2147483649 // fail
  a := +2147483649 // fail

end simple05.
