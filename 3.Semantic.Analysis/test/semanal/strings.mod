//
// strings.mod
//
// specific semantic anaysis for strings
// - definition of strings
// - string passing
// - type checking
//
// Note that the reference implementation
// - treats strings as character arrays
// - but does not support assignments of arrays
// This means that strings cannot be assigned to
// character arrays.

module strings;

var s: char[256];               // pass


procedure foo();
begin
  WriteStr("Hello, world!");    // pass
  WriteLn()
end foo;

procedure bar(str: char[]);     // pass
begin
  WriteStr(str);                // pass
  WriteLn()
end bar;

procedure foobar();
var s: char[9];
begin
  s := "No good.";              // fail
  WriteStr(s);                  // pass
  WriteLn()
end foobar;

procedure not_fun();
var s: char[6];
begin
  s[0]:='H'; s[1]:='e';         // pass
  s[2]:='l'; s[3]:='l';         // pass
  s[4]:='o'; s[5]:='\0';        // pass
  WriteStr(s);                  // pass
  WriteLn()
end not_fun;

begin
  foo();
  bar("This should work.");
  not_fun()
end strings.

