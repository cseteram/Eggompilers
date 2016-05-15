// hmtest00.mod
// this fail with redundant expression

module hmtest00;

var a : integer;

begin
    a + 1; // fail
    return
end hmtest00.
