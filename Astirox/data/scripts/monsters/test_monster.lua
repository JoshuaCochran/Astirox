t_monster = {
	sprite = {
		spriteSheet = "data/images/monster/demon0.png",
		spriteLeftPos = 32,
		spriteTopPos = 16,
		spriteWidth = 16,
		spriteHeight = 16
	},
	
	baseStats = {
		name = "test_demon",
		level = 1,
		minimumDamage = 3,
		maximumDamage = 5,
		maximumHP = 20,
		maximumMP = 15,
		defense = 15,
		--aggroRadius = 96; -- 6 Character lengths. (96/16 = 6)
		goldReward = 20,
		expReward = 31,
		miniboss = false,
		mainboss = false;
	}
}