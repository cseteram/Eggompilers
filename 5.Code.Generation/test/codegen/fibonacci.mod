//
// fibonacci
//
// fibonacci computation
//

module fibonacci;

var i : integer;

function fib(n: integer): integer;
begin
  if (n <= 1) then
    return n
  else
    return fib(n-1) + fib(n-2)
  end
end fib;

function ReadNumber(str: char[]): integer;
var i: integer;
begin
  WriteStr(str);
  i := ReadInt();
  return i
end ReadNumber;

begin
  WriteStr("Fibonacci numbers"); WriteLn();
  WriteLn();

  i := ReadNumber("Enter a number (0 to exit): ");

  while (i > 0) do
    WriteStr("fibonacci("); WriteInt(i); WriteStr(") = ");
    WriteInt(fib(i)); WriteLn();

    i := ReadNumber("Enter a number (0 to exit): ")
  end
end fibonacci.
