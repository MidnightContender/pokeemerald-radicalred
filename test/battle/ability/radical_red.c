#include "global.h"
#include "test/battle.h"

// Abilities ported from Radical Red

SINGLE_BATTLE_TEST("Feline Prowess doubles the Sp. Atk of special moves only", s16 damage)
{
    u32 move, ability;
    PARAMETRIZE { move = MOVE_WATER_GUN; ability = ABILITY_FELINE_PROWESS; }
    PARAMETRIZE { move = MOVE_WATER_GUN; ability = ABILITY_CUTE_CHARM; }
    PARAMETRIZE { move = MOVE_SCRATCH;   ability = ABILITY_FELINE_PROWESS; }
    PARAMETRIZE { move = MOVE_SCRATCH;   ability = ABILITY_CUTE_CHARM; }
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_SKITTY) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Striker boosts kicking moves by 30%", s16 damage)
{
    u32 move, ability;
    PARAMETRIZE { move = MOVE_JUMP_KICK; ability = ABILITY_STRIKER; }
    PARAMETRIZE { move = MOVE_JUMP_KICK; ability = ABILITY_SPEED_BOOST; }
    PARAMETRIZE { move = MOVE_SCRATCH;   ability = ABILITY_STRIKER; }
    PARAMETRIZE { move = MOVE_SCRATCH;   ability = ABILITY_SPEED_BOOST; }
    GIVEN {
        ASSUME(IsKickingMove(MOVE_JUMP_KICK));
        ASSUME(!IsKickingMove(MOVE_SCRATCH));
        PLAYER(SPECIES_BLAZIKEN) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.3), results[0].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Self Sufficient recovers 1/16th of Max HP at the end of each turn")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { Ability(ABILITY_SELF_SUFFICIENT); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SELF_SUFFICIENT);
        HP_BAR(player, damage: -(100 / 16));
    }
}

SINGLE_BATTLE_TEST("Self Sufficient does nothing at full HP")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { Ability(ABILITY_SELF_SUFFICIENT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SELF_SUFFICIENT);
    }
}

SINGLE_BATTLE_TEST("Blazing Soul gives Fire moves priority only at full HP")
{
    u32 hp, move;
    PARAMETRIZE { hp = 100; move = MOVE_EMBER; }
    PARAMETRIZE { hp = 99;  move = MOVE_EMBER; }
    PARAMETRIZE { hp = 100; move = MOVE_SCRATCH; }
    GIVEN {
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        ASSUME(GetMoveType(MOVE_SCRATCH) != TYPE_FIRE);
        PLAYER(SPECIES_TYPHLOSION) { Ability(ABILITY_BLAZING_SOUL); HP(hp); MaxHP(100); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        if (hp == 100 && move == MOVE_EMBER) {
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, move, player);
        }
    }
}

SINGLE_BATTLE_TEST("Frozen Mist blocks secondary effects")
{
    GIVEN {
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_NUZZLE, MOVE_EFFECT_PARALYSIS, 100) == TRUE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARTICUNO) { Ability(ABILITY_FROZEN_MIST); }
    } WHEN {
        TURN { MOVE(player, MOVE_NUZZLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NUZZLE, player);
        HP_BAR(opponent);
        NOT MESSAGE("The opposing Articuno is paralyzed, so it may be unable to move!");
    }
}

SINGLE_BATTLE_TEST("Frozen Mist prevents Stealth Rock and Spikes damage")
{
    u32 move;
    PARAMETRIZE { move = MOVE_STEALTH_ROCK; }
    PARAMETRIZE { move = MOVE_SPIKES; }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_STEALTH_ROCK) == EFFECT_STEALTH_ROCK);
        ASSUME(GetMoveEffect(MOVE_SPIKES) == EFFECT_SPIKES);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARTICUNO) { Ability(ABILITY_FROZEN_MIST); }
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Fatal Precision makes super effective moves always hit")
{
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) == 50);
        PLAYER(SPECIES_EELEKTROSS) { Ability(ABILITY_FATAL_PRECISION); }
        OPPONENT(SPECIES_SQUIRTLE);
    } WHEN {
        TURN { MOVE(player, MOVE_ZAP_CANNON, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Fatal Precision does not help moves that aren't super effective")
{
    GIVEN {
        PLAYER(SPECIES_EELEKTROSS) { Ability(ABILITY_FATAL_PRECISION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ZAP_CANNON, hit: FALSE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, player);
    }
}

SINGLE_BATTLE_TEST("Fatal Precision boosts super effective moves by 20%", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_FATAL_PRECISION; }
    PARAMETRIZE { ability = ABILITY_LEVITATE; }
    GIVEN {
        PLAYER(SPECIES_EELEKTROSS) { Ability(ability); }
        OPPONENT(SPECIES_SQUIRTLE);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.2), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Bull Rush boosts power by 20% on the first turn only")
{
    s16 firstTurn, secondTurn;
    GIVEN {
        PLAYER(SPECIES_BOUFFALANT) { Ability(ABILITY_BULL_RUSH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &firstTurn);
        HP_BAR(opponent, captureDamage: &secondTurn);
    } THEN {
        EXPECT_MUL_EQ(secondTurn, Q_4_12(1.2), firstTurn);
    }
}

SINGLE_BATTLE_TEST("Quill Rush raises Speed by 50% on the first turn only")
{
    GIVEN {
        PLAYER(SPECIES_DECIDUEYE_HISUI) { Ability(ABILITY_QUILL_RUSH); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(120); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
    }
}

SINGLE_BATTLE_TEST("Sage Power boosts special moves by 50%", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_SAGE_POWER; }
    PARAMETRIZE { ability = ABILITY_GLUTTONY; }
    GIVEN {
        PLAYER(SPECIES_SIMIPOUR) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Sage Power locks the user into its first move")
{
    GIVEN {
        PLAYER(SPECIES_SIMIPOUR) { Ability(ABILITY_SAGE_POWER); Moves(MOVE_WATER_GUN, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_SCRATCH, allowed: FALSE); MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
    }
}

SINGLE_BATTLE_TEST("Bone Zone lets bone moves hit Pokémon that are immune to them")
{
    u32 move, species;
    PARAMETRIZE { move = MOVE_BONEMERANG;  species = SPECIES_PIDGEY; }
    PARAMETRIZE { move = MOVE_SHADOW_BONE; species = SPECIES_RATTATA; }
    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Ability(ABILITY_BONE_ZONE); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Bone Zone ignores resistances to bone moves", s16 damage)
{
    u32 species;
    PARAMETRIZE { species = SPECIES_CHIKORITA; }
    PARAMETRIZE { species = SPECIES_RATTATA; }
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_CHIKORITA, 0) == TYPE_GRASS && GetSpeciesType(SPECIES_CHIKORITA, 1) == TYPE_GRASS);
        PLAYER(SPECIES_MAROWAK) { Ability(ABILITY_BONE_ZONE); }
        OPPONENT(species) { Defense(100); HP(300); MaxHP(300); }
    } WHEN {
        TURN { MOVE(player, MOVE_BONE_CLUB); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Bad Company prevents recoil damage")
{
    GIVEN {
        PLAYER(SPECIES_ARCHALUDON) { Ability(ABILITY_BAD_COMPANY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Bad Company stops the user's own moves from lowering its stats")
{
    GIVEN {
        PLAYER(SPECIES_ARCHALUDON) { Ability(ABILITY_BAD_COMPANY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CLOSE_COMBAT); }
        TURN { MOVE(player, MOVE_DRACO_METEOR); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Bad Company does not stop other Pokémon from lowering the user's stats")
{
    GIVEN {
        PLAYER(SPECIES_ARCHALUDON) { Ability(ABILITY_BAD_COMPANY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Phoenix Down revives the user at half HP the first time it faints")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { Ability(ABILITY_PHOENIX_DOWN); HP(1); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, hp: 0);
        ABILITY_POPUP(player, ABILITY_PHOENIX_DOWN);
        HP_BAR(player, hp: 100);
        MESSAGE("Ho-Oh rose from the ashes!");
        NOT MESSAGE("Ho-Oh fainted!");
    } THEN {
        EXPECT_EQ(player->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Phoenix Down only works once per battle")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { Ability(ABILITY_PHOENIX_DOWN); HP(1); MaxHP(2); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PHOENIX_DOWN);
        MESSAGE("Ho-Oh rose from the ashes!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        NOT ABILITY_POPUP(player, ABILITY_PHOENIX_DOWN);
        MESSAGE("Ho-Oh fainted!");
    }
}
