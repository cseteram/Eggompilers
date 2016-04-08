//
// test03
//
// parser test: syntax errors
// - module identifier does not match
// - function identifier does not match
// - procedure with a return type
// - function without a return type
// - extra semicolon at end of statement sequence
// - module body missing
//
// The errors must be eliminated one by one for testing since the compiler
// aborts at the first error.
//

// change "test03_wrong" -> "test03" to eliminate
module test03;

// change "foox" -> "foo" to eliminate
function foo(): integer;
begin
end foox;

// change "procedure" -> "function" to eliminate
procedure bar: integer;
begin
end bar;

// add return type to eliminate
function foobar();
begin
  // remove semicolon to eliminate
  return 1;
end foobar;

// add "begin" to eliminate
end test03_wrong.
