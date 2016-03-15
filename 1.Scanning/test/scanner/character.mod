module character;

var c: char;

function test(a: char): char;
begin
  return a
end test;

procedure WriteLn(string: char[]);
begin
end WriteLn;

begin
  c := '7';
  c := test(c);
  c := test('0');
  WriteLn("Hello, world!\n")
end character.

