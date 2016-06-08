//
// factorial
//
// factorial computation
//

module factorial;

var i : integer;

function fact(n: integer): integer;
begin
  if (n <= 0) then
    return 0
  else
    if (n <= 1) then
      return n
    else
      return n*fact(n-1)
    end
  end
end fact;

function ReadNumber(str: char[]): integer;
var i: integer;
begin
  WriteStr(str);
  i := ReadInt();
  return i
end ReadNumber;

begin
  WriteStr("Factorials"); WriteLn();
  WriteLn();

  i := ReadNumber("Enter a number (0 to exit): ");

  while (i > 0) do
    WriteStr("factorial("); WriteInt(i); WriteStr(") = ");
    WriteInt(fact(i)); WriteLn();

    i := ReadNumber("Enter a number (0 to exit): ")
  end
end factorial.
