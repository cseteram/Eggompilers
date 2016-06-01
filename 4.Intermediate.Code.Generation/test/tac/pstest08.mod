// find element in array

module heehoon;

var a : integer[111111];
    x,n,i : integer;

function find (arr : integer[]; elem : integer) : boolean;
var i,N : integer;
begin
    N := DIM(arr, 1);
    while (i < N) do
        if (arr[i] = elem) then
            return true
        end;
        i := i+1
    end;
    return false
end find;

begin
    n := ReadInt();
    
    i := 0;
    while (i < n) do
        a[i] := ReadInt()
    end;

    x := ReadInt();

    if (find(a,x)) then
        WriteStr("Found")
    else
        WriteStr("Not Found. I EGGER U")
    end;
    WriteLn()
end heehoon.
