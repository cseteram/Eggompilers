//
// allow simple folding of integer constants
//
module int_const_simple;

//
// simple(): fold negation with constant 'term'
//
// simple() interpretes the grammar is a slightly different way if a constant
// follows the initial sign of a simplexpr. simple() performs folding of nega-
// tion and constants iff the term follwing the +/- sign is a constant. This
// leads to unintuitive behavior since -MIN_INT + ... is accepted, but
// -MIN_INT * ... is not.
//
// To understand why consider the parse trees
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
// in the left case, the first term is represented by an CAstConstant node
// which can be easily folded with the "-" sign on its left. For the multi-
// plication, the (first) term is of type CAstBinaryOp and can thus not be
// folded anymore.
//
//       simpleexpr                       simpleexpr
//      /  |    |  \                        /   \
//    "-" term "+" term                   "-"  term
//     (of type)  (of type)                  (of type)
//  CAstConstant CAstConstant               CAstBinaryOp
//                                         /     |     \
//      ^                               factor  "*"  factor
//      |                              (of type)    (of type)
//  fold "-" w/ CAstConstant         CAstConstant  CAstConstant
//
procedure simple();
var i: integer;
begin
  i := -2147483648;                 // pass
  i := -2147483647;                 // pass
  i :=  2147483647;                 // pass
  i :=  2147483648;                 // fail

  i := -2147483648 + 2147483648;    // fail
  i := -2147483648 + -2147483648;   // fail
  i := -2147483648 + (-2147483648); // pass

  i := -2147483648 + 3;             // pass
  i := -2147483648 * 3;             // fail

  i := 0
end simple;

begin
end int_const_simple.

