//
// hmtest01
//
// arrays
//
// expected output: "435" "136524"
//

module hmtest01;

var a : integer[5];
    b : integer[5][5][5];
    i,j,k : integer;

procedure fun1(A : integer[]);
var B : integer[5];
begin
  i := 0;
  while (i < 5) do
    B[4 - i] := A[i];
    i := i + 1
  end;

  WriteInt(B[1]);
  WriteInt(B[2]);
  WriteInt(B[0]);
  WriteLn();
  return
end fun1;

procedure fun2(A : integer[][][]);
var B : integer[5][5][5];
begin
  i := 0;
  j := 0;
  k := 0;

  while (i < 5) do
    j := 0;
    while (j < 5) do
      k := 0;
      while (k < 5) do
        B[i][j][k] := A[k][j][i];
        k := k + 1
      end;
      j := j + 1
    end;
    i := i + 1
  end;

  WriteInt(B[1][0][0]);
  WriteInt(B[2][1][0]);
  WriteInt(B[3][2][1]);
  WriteInt(B[0][3][2]);
  WriteInt(B[1][0][1]);
  WriteInt(B[0][4][0]);
  WriteLn();
  return
end fun2;

begin
  i := 0;
  j := 0;
  k := 0;

  while (i < 5) do
    a[i] := i + 1;
    j := 0;
    while (j < 5) do
      k := 0;
      while (k < 5) do
        b[i][j][k] := i + j + k;
        k := k + 1
      end;
      j := j + 1
    end;
    i := i + 1
  end;

  fun1(a);
  fun2(b);

  return
end hmtest01.
