//
// array declaration and definitions
//

module arrays;

var A : integer[5];
    B : boolean[1];
    C : char[256];
    i : integer;
    b : boolean;
    c : char;

procedure print_array(array: integer[]);
begin
end print_array;

begin
  i := A[0];
  b := B[5];
  c := C[-1];

  print_array(A)
end arrays.
