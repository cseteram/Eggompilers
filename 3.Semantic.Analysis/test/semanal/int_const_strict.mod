//
// strict implementation of SnuPL/1's integer constants
//
module int_const_strict;

//
// strict(): 1:1 implementation of SnuPL/1 EBNF
//
// strict() does not perform folding of negation and constants.
// Since 'number' accepts only positive numbers, there is no way
// to directly represent MIN_INT.
//
procedure strict();
var i: integer;
begin
  i := -2147483648;                 // fail
  i := -2147483647;                 // pass
  i :=  2147483647;                 // pass
  i :=  2147483648;                 // fail

  i := 0
end strict;

begin
end int_const_strict.

