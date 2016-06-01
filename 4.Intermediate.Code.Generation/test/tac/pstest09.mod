// find 

module pstest04;

var a : integer[1111][1111];
    n, m, i, j : integer;

function sum (arr : integer[][]) : integer;
var ret, N, M, i, j : integer;
begin
    N := DIM(arr, 1);
    M := DIM(arr, 2);

    i := 0;
    while (i < N) do
        j := 0;
        while (j < M) do
            ret := ret + arr[i][j];
            j := j+1
        end;
        i := i+1
    end;
    return ret
end sum;

begin
    n := ReadInt();
    m := ReadInt();

    i := 0;
    while (i < n) do
        j := 0;
        while (j < m) do
            a[i][j] := ReadInt();
            j := j+1
        end;
        i := i+1
    end;

    WriteInt(sum(a));
    WriteLn()
end pstest04.
    
