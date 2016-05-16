// hmtest01.mod
// this fail with redundant expression

module hmtest01;

var a : integer[1];

begin
    a[0] := -2147483648;
    a[0] := -2147483648*1;
    a[0] := -(2147483648);
    a[0] := -(2147483648*1);
    return
end hmtest01.
