// hmtest01.mod
// this fail with redundant expression

module hmtest01;

var a : integer[1];

begin
    a[0]; // fail
    return
end hmtest01.
