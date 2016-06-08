//
// gcd
//
// greatest common divisor
// using the Euclidian algorithm
//

module gcd;

var n: integer[2];

// gcd(a: integer[2])
//
// computes the greatest common divisor
// subtraction-based version
function gcd_iter(a: integer[2]): integer;
begin
  while (a[0] # a[1]) do
    if (a[0] > a[1]) then a[0] := a[0] - a[1]
    else a[1] := a[1] - a[0]
    end
  end;
  return a[0]
end gcd_iter;

// gcd(a: integer[2])
//
// computes the greatest common divisor
// division-based version
function gcd_mod(a: integer[2]): integer;
var t: integer;
begin
  while (a[1] # 0) do
    t := a[1];
    a[1] := a[0] - a[0]/t*t;
    a[0] := t
  end;
  return a[0]
end gcd_mod;

// gcd(a: integer[2])
//
// computes the greatest common divisor
// recursive version
function gcd_rec(a: integer[2]): integer;
var t: integer;
begin
  if (a[1] = 0) then return a[0]
  else
    //return gcd_rec(a[1], a[0] - a/b*b)
    t := a[0];
    a[0] := a[1];
    a[1] := t - t/a[1]*a[1];
    return gcd_rec(a)
  end
end gcd_rec;

procedure ReadNumbers(n: integer[2]);
var i: integer;
begin
  WriteStr("Enter first number : ");
  n[0] := ReadInt();
  WriteStr("Enter second number: ");
  n[1] := ReadInt()
end ReadNumbers;

begin
  WriteStr("Greatest commond divisor"); WriteLn();
  WriteLn();

  ReadNumbers(n);

  WriteStr(" subtract  : "); WriteInt(gcd_iter(n)); WriteLn();
  WriteStr(" divide    : "); WriteInt(gcd_mod(n)); WriteLn();
  WriteStr(" recursive : "); WriteInt(gcd_rec(n)); WriteLn()
end gcd.
