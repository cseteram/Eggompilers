//
// semantics.mod
//
// semantic anaysis
// - constants assignments (range, type, unary operators)
// - variable definitions (use before def, multiple definitions)
// - function parameters (number, type)
// - type checking
//   - expressions: compatible operations/operands
//   - assignments: type of LHS = type of RHS
//   - return statements: correct type
//   - boolean types for conditions
//
// - does not test arrays and strings

module semantics;


var u,v: boolean;             // pass
    d,e: char;                // pass
    x,y: integer;             // pass
    a  : integer[10];         // pass
    b  : integer[5][10];      // pass
    str: char[256];           // pass
    d  : integer;             // fail
    t1 : integer[];           // fail


// empty procedure
procedure ProcedureCall();
begin
end ProcedureCall;

// empty function. This is considered correct code
// even though there is no return statement.
function FunctionCall(): integer;
begin
end FunctionCall;


// constant assignments and unary +/-/! operations
procedure Constants;
var a,b: boolean;
    c,d: char;
    i,j: integer;
    s  : char[14];
begin
  // boolean variables
  a := true;                  // pass
  a := false;                 // pass
  a := !true;                 // pass
  a := !!!true;               // pass
  a := !false;                // pass
  a := -true;                 // fail
  a := +false;                // fail
  a := 'a';                   // fail
  a := 0;                     // fail

  // character variables
  c := 'a';                   // pass
  d := '\n';                  // pass
  c := +'0';                  // fail
  c := true;                  // fail
  d := 0;                     // fail

  i := -0;                    // pass
  i := +0;                    // pass
  i := --0;                   // fail
  i := +-0;                   // fail
  i := -2147483648;           // pass (min int)
  j :=  2147483647;           // pass (max int)
  i := -2147483649;           // fail (<min int)
  j :=  2147483648;           // fail (>max int)
  i := true;                  // fail
  i := 'a';                   // fail

  a := true
end Constants;


// variable definitions
procedure UseBeforeDef(k: integer);
var i,j: integer;
begin
  i := j;                     // pass
  i := k;                     // pass
  i := x;                     // pass
  i := z;                     // fail

  i := 0
end UseBeforeDef;


// multiple definitions
procedure MultipleDef(i,i: integer); // fail (parameter - parameter)
var i: integer;               // fail (parameter - local var)
    m,m: integer;             // fail (local var - local var)
begin
  i := 1
end MultipleDef;


// parameters: too many/few/wrong type of parameters
procedure Parameters(p1, p2: integer);
begin
  Parameters();             // fail
  Parameters(1);            // fail
  Parameters(true, false);  // fail
  Parameters(1,2,3);        // fail
  Parameters(1,2,3,4);      // fail

  Parameters(p2,p1);        // pass
  Parameters(1,2)           // pass
end Parameters;


// type checks
procedure Expressions(p1, p2, p3, p4: integer);
var a,b,c: boolean;
    i,j,k: integer;
    A    : integer[10];
begin
  a := 1 + true;            // fail
  a := true + false;        // fail
  a := b + c;               // fail
  a := a > b;               // fail
  a := !!!b;                // pass
  a := a && (!b);           // pass

  i := j + b;               // fail
  i := j && k || p1;        // fail
  i := !j;                  // fail
  i := j + -k;              // fail
  i := j + (-k);            // pass

  a := a && !b && (i < j)   // pass
         && (j < k)
         || (i = k)
end Expressions;


// type checks involving arraysk
procedure Arrays();
var a: boolean;
    i: integer;
    A: integer[10];
    B: integer[10][20];
    D: char[5];
    E: boolean[20];
begin
  a := i[0];                  // fail
  a[0] := true;               // fail
  A[0] := i;                  // pass
  A[-1] := B[0][0];           // pass
  A[0][0] := i;               // fail
  B[0][0] := i;               // pass
  D[1] := A[0];               // fail
  E[0] := B[A[0]][A[1]] > 0   // pass
end Arrays;


// assignment type checks
procedure Assignments(p1, p2, p3, p4: integer);
var a,b,c: boolean;
    i,j,k: integer;
begin
  a := i;                   // fail
  i := a;                   // fail
  i := ProcedureCall()      // fail
end Assignments;


// return statements type checks
procedure ProcReturn();
begin
  return 5                  // fail
end ProcReturn;

function NoReturn(): integer;
begin
  return                    // fail
end NoReturn;

function IntReturn(): integer;
begin
  return 1 > 2              // fail
end IntReturn;

function BoolReturn(): boolean;
begin
  return 1 + 2              // fail
end BoolReturn;


// condition type checking
procedure If(p1, p2: integer);
begin
  if (p1 + p2 > 0) then         // fail
    return
  else
    return
  end;

  if (ProcedureCall()) then // fail
  //if (FunctionCall() > 0) then // fail
    return
  else
    return
  end
end If;

procedure While();
var a,b: integer;
begin
  while (a)    // fail
    b := b-1
  end
end While;


begin
  u := v;                   // pass
  x := y+1                  // pass
end semantics.
