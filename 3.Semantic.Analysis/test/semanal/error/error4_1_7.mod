module invalid;

var a : integer;

begin
    a := 2147483647; // pass
    a := 2147483648; // fail (typecheck)
    a := 12346734652445364374623765345245647834735; // fail (parser)
    a := 1243513463764'u'124565376421234; // fail (parser)
    return
end invalid.
