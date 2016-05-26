// pstest06
// fibonacci seq.

module pstest06;

function fib(n : integer) : integer;
begin
    if (n < 0) then
        WriteStr("number cannot be a negative integer");
        return -1
    else
        if (n = 0) then
            return 0
        else 
            if (n = 1) then
                return 1
            else 
                return fib(n-2) + fib(n-1)
            end
        end
    end
end fib;

begin 

end pstest06.
