// hmtest04.mod
// Weak points in reference parser
// abuse of stringConstant

module weak;                    // fail everywhere

var _str_2 : integer;           // symbol table occupied
    _str_3 : char[7];           // symbol table occupied

procedure _str_1(s : char[]);   // symbol table occupied
begin
  WriteStr(s);
  return
end _str_1;

procedure clear(s : char[]);
begin
  s[0] := '\0'
end clear;

begin
  _str_3[0] := '_';
  _str_3[1] := 'f';
  _str_3[2] := 'u';
  _str_3[3] := 'n';
  _str_3[4] := '_';
  _str_3[5] := '3';
  _str_3[6] := '\0';

  WriteStr("_str_1");           // can't use _str_1 as pointer to array of char
  _str_1("_str_2");             // can't use _str_2 as pointer to array of char
  _str_1("_str_3");             // when using _str_3, _fun_3 will be returned
  WriteStr("_str_4");
  _str_4[5] := '5';             // using undeclared variable _str_4

  clear("constant?");           // stringConstant is lie - it is mutable
  return
end weak.

