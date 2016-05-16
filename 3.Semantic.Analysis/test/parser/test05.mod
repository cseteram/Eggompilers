//
// test05
//
// parser test:
// - test integer constant limits
//   (its okay to catch (+)2147483648 later during type checking)
//
module test05;

var min, max: integer;

begin
  // smallest valid integer constant
  min := -2147483648;

  // largest valid integer constant
  max :=  2147483647
end test05.
