t_player = {
	baseStats = {
	 strength = 5,
	 dexterity = 5,
	 intelligence = 5,
	 wisdom = 5,
	 stamina = 5
	},
	
	modifiers = {
		str_modifier = 1,
		dex_modifier = 1,
		int_modifier = 1,
		wis_modifier = 1,
		stam_modifier = 1,
		defense_modifier = 1,
		res_modifier = 1,
		hp_modifier = 1,
		mp_modifier = 1,
		weapon_damage_modifier = 1,
		burned_enemy_modifier = 1,
		drop_rate_modifier = 1,
		xp_modifier = 1,
		jp_modifier = 1
		},
		
	sprite = {
		spriteSheet = "data/images/player/humanoid1.png",
		spriteLeftPos = 0,
		spriteTopPos = 0,
		spriteWidth = 16,
		spriteHeight = 16
	}
}

calculateHP = function()
	return (player:BaseStat("stamina") + player:GearStat("stamina")) * 10 * player:StatMod("hp")
end

calculateMP = function()
	return (player:BaseStat("intelligence") + player:GearStat("intelligence")) * 10 * player:StatMod("mp")
end

calculatePhysicalAttack = function()
	return math.ceil(player:BaseStat("weapon damage") * (player:BaseStat("strength") + (player:BaseStat("dexterity") * 0.8)))
end

calculateMagicAttack = function()
	return math.ceil(player:BaseStat("intelligence") * 0.8)
end

calculateXPToLevel = function()
	return math.pow(player:BaseStat("level"), 3) + 30
end

calculateResistance = function()
	return math.ceil((player:BaseStat("intelligence") * 0.05) + (player:BaseStat("wisdom") * 0.1) + player:GearStat("resistance")) * player:StatMod("resistance")
end

calculateDefense = function()
	return math.ceil((player:BaseStat("strength") * 0.05) + (player:BaseStat("stamina") * 0.1) + player:GearStat("defense")) * player:StatMod("defense")
end