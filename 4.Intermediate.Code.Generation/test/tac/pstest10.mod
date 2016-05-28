// pstest10
// https://www.acmicpc.net/problem/12790

module MiniFantasyWar;
var T, tc, i, ans : integer;
	input : integer[8];

function solve(stats : integer[]) : integer;
var hp, ehp, mp, emp, atk, eatk, def, edef : integer;
	ret : integer;
begin
	hp := stats[0]; ehp := stats[1]; mp := stats[2]; emp := stats[3];
	atk := stats[4]; eatk := stats[5]; def := stats[6]; edef := stats[7];

	ret := 0;
	if (hp + ehp >= 1) then
		ret := ret + hp + ehp
	else
		ret := ret + 1
	end;
	if (mp + emp >= 1) then
		ret := ret + 5 * (mp + emp)
	else
		ret := ret + 5 * 1
	end;
	if (atk + eatk >= 0) then
		ret := 2 * (atk + eatk)
	end;
	ret := ret + 2 * (def + edef);

	return ret
end solve;

begin
	T := ReadInt();

	tc := 0;
	while (tc < T) do
		i := 0;
		while (i < 8) do
			input[i] := ReadInt();
			i := i + 1
		end;

		ans := solve(input);
		WriteInt(ans);
		WriteLn();		

		tc := tc + 1
	end
end MiniFantasyWar.
