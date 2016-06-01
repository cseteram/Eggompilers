//
// prime factors
//

module pfact;

var n: integer;

procedure primefactor(n: integer);
var f: integer;
begin
  if (n < 1) then return
  else
    if (n = 1) then WriteStr(" "); WriteInt(1)
    else
      f := 2;
      while (f <= n) do
        if (n/f*f = n) then
          WriteStr(" "); WriteInt(f);
          n := n/f
        else
          f := f + 1
        end
      end
    end
  end
end primefactor;

begin
  WriteStr("Prime factoring"); WriteLn();
  WriteLn();
  WriteStr("Enter number to factor: "); n := ReadInt();
  WriteStr("  prime factors of "); WriteInt(n); WriteStr(": ");
  primefactor(n); WriteLn()
end pfact.
