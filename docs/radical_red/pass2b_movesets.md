# Pass 2b: movesets

Movesets now follow the Radical Red dex ([JwowSquared/Radical-Red-Pokedex](https://github.com/JwowSquared/Radical-Red-Pokedex)).

## What changed

- **Level-up learnsets** (`src/data/pokemon/level_up_learnsets/gen_9.h`, the set the fork uses): 1,075 learnsets were replaced with RR's list, levels included. Level 0 entries are evolution moves, as in RR.
- **Egg moves** (`src/data/pokemon/egg_moves.h`): 210 lists were replaced with RR's. Where RR lists no egg moves for a species, the fork's list stays.
- **TM and tutor moves**: RR's TM and tutor compatibility is in a new learnset source, `tools/learnset_helpers/porymoves_files/radicalred.json`. The build already makes a species able to be taught any move it learns in any official game, so this file only adds to those lists; nothing was removed. About 2,900 species/move pairs were added, mostly Toxic, Aura Sphere, Snowscape, Feint Attack, Cut and Strength.
- **Engine limits**: RR learnsets run up to 31 level-up moves and 20 egg moves. The move relearner and the HGSS dex only read the first 20 level-up moves and 19 egg moves, so `MAX_LEVEL_UP_MOVES` is now 40 and `EGG_MOVES_ARRAY_COUNT` is 24. Before this, 168 species would have had late moves missing from the relearner.

## Forms that share a learnset

- **Rotom**: all forms share one learnset in the fork, so it uses base Rotom's. The form moves (Overheat, Hydro Pump and so on) still come from the form change.
- **Zygarde**: 10% and 50% share one learnset; it uses Zygarde 50%'s.
- **Basculin White-Striped** isn't in the RR dex, so its moves are unchanged.

## RR-only moves left out (waiting on pass 3)

| Move | Level-up | Other |
| --- | --- | --- |
| Aqua Fang | Basculin (42), Bruxish (48), Drednaw (72), Feraligatr (45), Gyarados (30), Huntail (50), Sharpedo (75) | |
| Draco Barrage | Guzzlord (77), Hydreigon (85), Kommo-o (77), Tropius (52) | TM for 111 Pokémon |
| Dark Hole | Darkrai (81) | Tutor for 58 Pokémon |
| Soul Robbery | Mewtwo (100) | |
| Forbidden Spell | Chimecho (50), Unown (34) | |
| Sonic Slash | Ninjask (54) | |
| Soupercell Slam | | TM for 68 Pokémon |

Unown now learns Hidden Power (1), Mystical Power (22) and Ancient Power (47).
