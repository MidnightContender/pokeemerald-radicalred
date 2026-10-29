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
