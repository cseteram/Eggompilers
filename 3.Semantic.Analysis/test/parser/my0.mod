module my0;

var A: integer[10];
    i: integer;

procedure merge(A: integer[]; i,j: integer);
var B: integer[10];
    p,mid,fig1,fig2: integer;
begin
  p := i;
  mid := (i + j) / 2;
  fig1 := i;
  fig2 := mid + 1;

  while ((p <= j) && (fig1 <= mid) && (fig2 <= j)) do
    if (A[fig1] > A[fig2])
    then
      B[p] := A[fig2];
      fig2 := fig2 + 1
    else
      B[p] := A[fig1];
      fig1 := fig1 + 1
    end;
    p := p + 1
  end;

  if (fig1 > mid) then
    while (fig2 <= j) do
      B[p] := A[fig2];

      p := p + 1;
      fig2 := fig2 + 1
    end
  end;

  if (fig2 > j) then
    while (fig1 <= mid) do
      B[p] := A[fig1];

      p := p + 1;
      fig1 := fig1 + 1
    end
  end

end merge;

procedure mergesort(A: integer[]; i,j: integer);
var mid: integer;
begin
  if (!(i < j)) then
    return
  end;

  mid := (i + j) / 2;

  mergesort(A, i, mid); 
  mergesort(A, mid + 1, j);

  merge(A, i, j)
end mergesort;

procedure Init(A: integer[]);
begin
  i := 0;
  while (i < 10) do
    A[9 - i] := i
  end;
  mergesort(A, 0, 9)
end Init;

procedure Print(A: integer[]);
var N: integer;
begin
  N := DIM(A, 1);

  i := 0;
  while (i < N) do
    WriteInt(A[i])
  end
end Print;

begin
  Init(A);
  Print(A)
end my0.
