//
// char declaration and definitions
//
// does not require array or string support
//

module char02;

var c: char;

procedure compare(c: char);
begin
  if (c >= '1') then WriteChar('1')
  else WriteChar('0') end;
  WriteLn()
end compare;

begin
  c := '1';
  compare(c);
  compare('a');
  compare('1');
  compare('\0')
end char02.
