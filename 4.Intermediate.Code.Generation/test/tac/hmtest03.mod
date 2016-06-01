//
// hmtest03
//
// IR generation
//

module hmtest03;

var a : integer[5];
    b : integer[5][5][5];

procedure fun1(A : integer);
begin
end fun1;

procedure fun2(A : integer[]);
begin
end fun2;

procedure fun3(A : integer[][]);
begin
end fun3;

procedure fun4(A : integer[][][]);
begin
end fun4;

begin
    fun1(a[2]);
    fun1(b[2][3][4]);
    fun2(a);
    fun2(b[2][3]);
    fun3(b[2]);
    fun4(b);
    return
end hmtest03.
