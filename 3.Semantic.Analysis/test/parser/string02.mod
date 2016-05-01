//
// strings
//

module string02;

var age, year: integer;

begin
  WriteStr("Enter your age: "); age := ReadInt();
  WriteStr("Enter the year: "); year := ReadInt();

  WriteStr("You will be 100 years old in the year ");
  WriteInt(year + 100 - age);
  WriteStr(".\n")
end string02.
