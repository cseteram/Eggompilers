module pstest13;

var cond1 : boolean[5];
	cond2 : boolean[7][3];
	cond3 : boolean[9][4][8];
	cond4 : boolean[11][2][3][5];
	ret : boolean;

function f
	(c1 : boolean[]; c2 : boolean[][];
	 c3 : boolean[][][]; c4 : boolean[][][][5]) : boolean;
begin
	if (c1[3] || c2[3][2] || c3[3][2][1] || c4[3][2][1][0]) then
		if (c1[3]) then
			return !c1[3]
		else
			return !(c2[3][2] && c3[3][2][1])
		end
	else
		return !(c3[3][2][1] || c4[3][2][1][0])
	end;
	return !c3[3][2][1];
	return !f(c1, c2, c3, c4);
	return f(c1, c2, c3, c4)
end f;

begin
	while (f(cond1, cond2, cond3, cond4)) do
		while (!f(cond1, cond2, cond3, cond4)) do
			if (cond1[5]) then
				ret := !cond3[1][3][2]
			else
				ret := cond3[3][2][1] && cond2[1][2]
			end
		end;
		while(!cond2[3][4]) do
			while(!cond4[9][8][7][6]) do
				if (cond3[1][3][2]) then
					ret := !true || false
				else
					ret := !true && true
				end
			end
		end
	end;
	if (ret && cond1[0]) then
		WriteStr("sound, compact");
		WriteLn();
		WriteInt(1)
	else
		if (ret && !cond1[0]) then
			WriteStr("sound, not compact");
			WriteLn();
			WriteInt(1)
		else 
			if (!ret && cond1[0]) then
				WriteStr("not sound, compact");
				WriteLn();
				WriteInt(1)
			else
				WriteStr("not sound, not compact");
				WriteLn();
				WriteInt(0)
			end
		end
	end;

	return
end pstest13.
