//
// primes
//
// compute prime numbers
// (not in a very efficient way)
//

module primes;

var n, pn: integer;
    p: integer[1000000];

function CalcPrimes(p: integer[]; n: integer): integer;
var N, pidx, psqrt, v, i: integer;
    isprime: boolean;
begin
  WriteStr("  computing primes from 1 to "); WriteInt(n); WriteStr("...");

  N := DIM(p, 1);       // size of array
  if (N < 1) then return 0 end;


  p[0] := 1;            // 1 is prime
  p[1] := 2;            // 2 is prime
  pidx := 2;            // index of next prime in p[]

  psqrt := 1;           // index of prime used as square root

  v := 3;               // next number to test

  while (v <= n) do
    // check whether v is prime by dividing it by all primes
    // up to p[psqrt]
    isprime := true;
    i := 1;
    while (isprime && (i <= psqrt)) do
      if (v/p[i]*p[i] = v) then
        isprime := false
      end;
      i := i + 1
    end;

    // if v is prime store it in p[]
    if (isprime = true) then
      p[pidx] := v;
      pidx := pidx + 1;
      if (pidx = N) then
        // reached end of array, stop
        WriteStr("WARNING: array too small to hold all primes.");
        n := 0
      end
    end;

    v := v + 2;

    if (v > p[psqrt]*p[psqrt]) then
      psqrt := psqrt + 1;
      if (psqrt >= pidx) then
        WriteStr("ERROR.");
        n := 0
      end
    end
  end;

  WriteStr("done. "); WriteInt(pidx); WriteStr(" primes found."); WriteLn();
  return pidx
end CalcPrimes;

procedure PrintPrimes(p: integer[]; n, pn: integer);
var i: integer;
begin
  WriteStr("Prime numbers 1 to "); WriteInt(n); WriteLn();
  i := 0;
  while (i < pn) do
    WriteStr("  "); WriteInt(p[i]);
    i := i + 1;
    if (i/8*8 = i) then WriteLn() end
  end;
  WriteLn()
end PrintPrimes;

begin
  WriteStr("Prime numbers"); WriteLn();
  WriteStr("Compute primes up to : "); n := ReadInt();
  pn := CalcPrimes(p, n);
  PrintPrimes(p, n, pn)
end primes.
