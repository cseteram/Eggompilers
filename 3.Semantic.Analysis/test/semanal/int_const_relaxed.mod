//
// advanced folding of integer constants
//
module int_const_relaxed;

//
// relaxed(): fold negation with constant.
//
// relaxed() improves the unintuitive behavior of simple() by also folding an
// integer constant if it appears as the leftmost leaf in the 'term' expression
// following the inital sign in 'simpleexpr'.
//
// I.e., for the parse trees
//
//     -2147483648 + 5                  -2147483648 * 5
//
//       simpleexpr                       simpleexpr
//      /  |    |  \                        /   \
//    "-" term "+" term                   "-"   term
//         |        |                          /  |  \
//       factor   factor                 factor  "*"  factor
//         |        |                      |             |
//       number   number                 number       number
//         |        |                      |             |
//    2147483648    5                 2147483648         5
//
//
// we first identify the leftmost leaf in the first term and fold if it is
// a CAstConstant:
//
//       simpleexpr                       simpleexpr
//      /  |    |  \                        /   \
//    "-" term "+" term                   "-"  term
//     (of type)  (of type)                  (of type)
//  CAstConstant CAstConstant               CAstBinaryOp
//      ^                                  /     |     \
//      |                               factor  "*"  factor
//  fold "-" w/ CAstConstant          (of type)    (of type)
//                                   CAstConstant  CAstConstant
//                                       ^
//                                       |
//                                   fold "-" w/ CAstConstant
//
// This approach requires modifications to the AST data structure since we need
// to know whether an expression had been enclosed in parentheses or not.
// Otherwise we would wrongfully accept expressions such as
//
//     i := -(2147483648 + 5)
//
// or even worse (b is of boolean type here)
//
//     b := -(1 > 0)
//
procedure relaxed();
var i: integer;
    b: boolean;
begin
  i := -2147483648;                 // pass
  i := -2147483647;                 // pass
  i :=  2147483647;                 // pass
  i :=  2147483648;                 // fail

  i := -2147483648 + 2147483648;    // fail
  i := -2147483648 + -2147483648;   // fail
  i := -2147483648 + (-2147483648); // pass

  i := -2147483648 + 3;             // pass
  i := -2147483648 * 3;             // pass
  i := -(2147483648 + 3);           // fail
  i := -(i + 2147483648);           // fail

  i := -(2147483648);               // fail
  i := -(-(-(1)));                  // pass

  b := -((1 > 0) && (2 < 5));       // fail

  i := 0
end relaxed;

begin
end int_const_relaxed.

