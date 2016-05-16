module while_if;

var a, sum : integer;

begin
    a := ReadInt();
    
    sum := 0;
//    while (a) do
//        sum := sum + a;
//        a := a-1
//    end;
    if (a) then
        sum := sum + a
    else
        sum := a
    end;

    WriteInt(sum)
end while_if.
