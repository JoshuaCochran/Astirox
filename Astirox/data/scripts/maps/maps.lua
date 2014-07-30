initPortals = function()
local count = 0;
	for k,v in pairs(connectsTo) do
		map:AddTargetMap(k)
		for m,d in pairs(v) do
			if (m == "x1") then
				map:AddExitPosX(d)
			elseif (m == "y1") then
				map:AddExitPosY(d)
			elseif (m == "x2") then
				map:AddTargetPosX(d)
			elseif (m == "y2") then
				map:AddTargetPosY(d)
			end
		end
	end
end

initSpawn = function()
local totalRarity = 0;
	for k,v in pairs(t_monsters) do
		totalRarity = totalRarity + v
	end
	for k,v in pairs(t_monsters) do
		map:AddSpawnCandidate(k, (v/totalRarity))
	end
end