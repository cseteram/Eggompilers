module array;
var a : boolean[5];
    i : integer;
    cnt : integer;

begin
    cnt := 0;

    i := 0;
    while (i < 5) do
    //    if (a[i]) then // pass
    //    if (a['c']) then // fail
    //    if (a[true]) then // fail
        if (a["hello"]) then // fail
            cnt := cnt + 1
        else
            cnt := cnt
        end;
        i := i+1
    end;

    WriteInt(cnt)
end array.
