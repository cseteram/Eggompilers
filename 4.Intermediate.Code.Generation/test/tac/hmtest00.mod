// hmtest03.mod

module hmtest03;

// empty procedure
procedure ProcedureCall();
begin
end ProcedureCall;

// condition type checking
procedure If(p1, p2: integer);
begin
  if (p1 + p2 > 0) then         // pass
    return
  else
    return
  end;

  if (true) then                // pass
    return
  else
    return
  end
end If;

begin
  If(1, 2)
end hmtest03.
