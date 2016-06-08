//
// hmtest02
//
// arrays of string
//
// expected output: "Hello2Hello1Hello4Hello3" "Hello, World!"
//

module hmtest02;

var g_arr : char[7][4];

procedure split(str_arr : char [7][4]);
var arr1, arr2, arr3, arr4 : char[7];
    i, j : integer;
begin
  i := 0;
  while (i < 7) do
    arr2[i] := str_arr[i][0];
    arr1[i] := str_arr[i][1];
    arr4[i] := str_arr[i][2];
    arr3[i] := str_arr[i][3];
    i := i + 1
  end;

  WriteStr(arr1);
  WriteStr(arr2);
  WriteStr(arr3);
  WriteStr(arr4);
  WriteLn();
  return
end split;

function merge(
  str1 : char[7];
  str2 : char[7];
  str3 : char[7];
  str4 : char[7]
) : boolean;
var str_arr : char[7][4];
    i : integer;
begin
  i := 0;
  while (i < 7) do
    str_arr[i][0] := str1[i];
    str_arr[i][1] := str2[i];
    str_arr[i][2] := str3[i];
    str_arr[i][3] := str4[i];
    i := i + 1
  end;

  split(str_arr);
  return true
end merge;

begin
  if (merge("Hello1", "Hello2", "Hello3", "Hello4")) then
    WriteStr("Hello, World!");
    WriteLn()
  end

end hmtest02.
