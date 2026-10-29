#include "global.h"
#include "test/battle.h"

// Moves ported from Radical Red

SINGLE_BATTLE_TEST("Soupercell Slam hurts the user by half its max HP on a miss")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SOUPERCELL_SLAM) == EFFECT_RECOIL_IF_MISS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SOUPERCELL_SLAM, hit: FALSE); }
    } SCENE {
        s32 maxHP = GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP);
        MESSAGE("Wobbuffet used Soupercell Slam!");
        MESSAGE("Wobbuffet's attack missed!");
        MESSAGE("Wobbuffet kept going and crashed!");
        HP_BAR(player, damage: maxHP / 2);
    }
}

SINGLE_BATTLE_TEST("Dark Hole has a 40% chance to put the target to sleep")
{
    PASSES_RANDOMLY(40, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_DARK_HOLE, MOVE_EFFECT_SLEEP, 40) == TRUE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_HOLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_HOLE, player);
        MESSAGE("The opposing Wobbuffet fell asleep!");
    }
}

SINGLE_BATTLE_TEST("Soul Robbery steals the target's stat boosts before attacking")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SOUL_ROBBERY) == EFFECT_SPECTRAL_THIEF);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_SOUL_ROBBERY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        MESSAGE("Wobbuffet used Soul Robbery!");
        MESSAGE("Wobbuffet stole the target's boosted stats!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Leech Fang restores half the damage dealt")
{
    s16 damage;
    s16 healed;
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_LEECH_FANG) == EFFECT_ABSORB);
        ASSUME(IsBitingMove(MOVE_LEECH_FANG));
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LEECH_FANG); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("The opposing Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-0.5), healed);
    }
}

SINGLE_BATTLE_TEST("Aqua Fang is a biting move boosted by Strong Jaw", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_STRONG_JAW; }
    PARAMETRIZE { ability = ABILITY_ROCK_HEAD; }
    GIVEN {
        ASSUME(IsBitingMove(MOVE_AQUA_FANG));
        PLAYER(SPECIES_TYRANTRUM) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_AQUA_FANG); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Sonic Slash does more damage the faster the user is", s16 damage)
{
    u32 speed;
    PARAMETRIZE { speed = 100; }
    PARAMETRIZE { speed = 400; }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SONIC_SLASH) == EFFECT_ELECTRO_BALL);
        ASSUME(GetMoveCategory(MOVE_SONIC_SLASH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Speed(speed); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_SLASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Draco Barrage hits Fairy types and hurts the user by a third of the damage")
{
    s16 damage, recoil;
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_CLEFAIRY, 0) == TYPE_FAIRY);
        PLAYER(SPECIES_HYDREIGON);
        OPPONENT(SPECIES_CLEFAIRY);
    } WHEN {
        TURN { MOVE(player, MOVE_DRACO_BARRAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRACO_BARRAGE, player);
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &recoil);
    } THEN {
        EXPECT_GT(damage, 0);
        EXPECT_EQ(recoil, damage * 33 / 100);
    }
}

SINGLE_BATTLE_TEST("Draco Barrage becomes physical when the user's Attack is higher", s16 damage)
{
    u32 move;
    PARAMETRIZE { move = MOVE_REFLECT; }
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    GIVEN {
        PLAYER(SPECIES_HYDREIGON) { Attack(300); SpAttack(50); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_DRACO_BARRAGE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Forbidden Spell calls one of its eight moves at random")
{
    GIVEN {
        PLAYER(SPECIES_CHIMECHO);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FORBIDDEN_SPELL, WITH_RNG(RNG_METRONOME, 0)); }
    } SCENE {
        MESSAGE("Chimecho used Forbidden Spell!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SMASH, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}
