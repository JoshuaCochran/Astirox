Starting_Map = {
	filename = map.tmx,
	mobCount = 5,
	spawnLocs = {
		spawn1 = {
			x = 24,
			y = 24
		},
		spawn2 = {
			x = 156,
			y = 40
		},
		spawn3 = {
			x = 24,
			y = 40
		}
		spawn4 = {
			x = 256,
			y = 256
		}
		spawn5 = {
			x = 34,
			y = 34
		}
	},
	t_monsters = {
		test_monster.lua = 100,
		test_monster2.lua = 200;
	}
}

checkSpawn = function()
{
	local totalRarity = 0;
	if map:GetNumAliveMobs()<Starting_Map["mobCount"] then 
		for k,v in pairs(Starting_Map[t_monsters]) do
			totalRarity = totalRarity + v
		end
		for k,v in pairs(Starting_Map[t_monsters]) do
			map:AddSpawnCandidate(k, (v/TotalRarity))
		end
		map:SpawnMonster()
	end
}

--[[ 
C++ code

void Map::AddSpawnCandidate(std::string filename, double rarity)
{
	SpawnCandidates.push_back(filename);
	SpawnCandidateRarities.push_back(rarity);
}

void Map::SpawnMonster()
{
	boost::random::discrete_distribution<> dist(SpawnCandidateRarities.begin(), SpawnCandidateRarities.end());
	Monster* mon = new Monster(SpawnCandidates[dist(Game::rng)]);
	spawnedMonsters.push_back(mon);
	for (int i = 0; i < SpawnCandidateRarities.size(); i++)
		SpawnCandidateRarities.pop_back();
	for (int i = 0; i < SpawnCandidates.size(); i++)
		SpawnCandidates.pop_back();
}
--]]
	