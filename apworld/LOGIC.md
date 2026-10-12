# Progression logic

A world is opened by holding its access item, so the five worlds hang off Menu
rather than off each other and can be received in any order. Edo needs nothing;
the player starts there.

Inside a world the access item opens every level at once, so the levels hang off
a hub rather than chaining. Only the gates that are real in game sit between a
hub and a level. The castle stays behind its checkpoint's entry pass count.

Level names and the in-world gates come from the save format and the
walkthroughs; see `lib/gga/docs/save_memory_map.md` in the Goemon64Recomp repo.

```
Menu
 ├─ Edo                       (free)
 │   ├─ Home of the Wiseman
 │   ├─ Lost'n Road
 │   ├─ Lost'n Town
 │   ├─ Ringbell Pass         [Chain Pipe]
 │   ├─ Digadig Gold Mine
 │   └─ Edo Checkpoint  (5)   └─ Edo Castle
 │
 ├─ Ryugu                     [Ryugu Access]
 │   ├─ Kappa Road            (route to Naruto Road needs [Yae + Chain Pipe])
 │   ├─ Naruto Road           [Yae + Chain Pipe]
 │   ├─ Frog Mountain
 │   ├─ Otohime Town          (Yae joins here)
 │   └─ Ryugu Checkpoint (10) └─ Ryugu Castle  [Yae]
 │
 ├─ Mafu Island               [Mafu Island Access]
 │   ├─ Kokeshi Road          [Chain Pipe]
 │   ├─ Maneater Road
 │   ├─ Pochan Lake           [Yae]
 │   ├─ Tomb Pass
 │   ├─ Spook Village
 │   └─ Mafu Checkpoint (15)  └─ Majin Castle
 │
 ├─ Underworld                [Underworld Access]
 │   ├─ Kaiware Road
 │   ├─ Mokeke Forest
 │   ├─ Wonder Valley
 │   ├─ Tree Stump Village
 │   ├─ Creep Village         [Yae]
 │   └─ Underworld Checkpoint (20) └─ Underworld Castle
 │
 └─ Floating Castle           [Floating Castle Access]
     ├─ Cloud Fortress
     ├─ Sky Garden
     ├─ Wind Castle Wall
     ├─ Floating World Town
     └─ Floating Checkpoint (25) └─ Dream Castle   = goal
```

## Items

| Item | Count | Role |
| --- | --- | --- |
| Entry Pass | fills the pool | counted requirement at each checkpoint |
| Chain Pipe | 1 | bought from Kenzo in Lost'n Town |
| Yae | 1 | joins in Otohime Town; lets the party cross water |
| World access | 4 | Ryugu, Mafu Island, Underworld, Floating Castle |

44 entry passes exist in the game, by the cumulative cap table at
`D_8010AEB8_5C60D8`. The pool places as many as there is room for after the
unique progression items.

## Gates

| Gate | Requirement |
| --- | --- |
| Edo Checkpoint | 5 entry passes |
| Ryugu Checkpoint | 10 |
| Mafu Checkpoint | 15 |
| Underworld Checkpoint | 20 |
| Floating Checkpoint | 25 |
| Ringbell Pass | Chain Pipe, for the star blocks |
| Kokeshi Road | Chain Pipe, star blocks on the way to its pass |
| Naruto Road | Yae and Chain Pipe: dive in, surface, then star blocks |
| Pochan Lake | Yae |
| Creep Village | Yae, reached by diving through Mokeke Forest |
| Ryugu Castle | Yae |
| Dream Castle | Yae |
| Kappa Road: Stage Clear to Naruto Road | Yae and Chain Pipe |
| Ryugu, Mafu Island, Underworld, Floating Castle | that world's access item |

Dropping Chain Pipe removes seven checks, dropping Yae removes eight and Dream
Castle with them, so both are required to finish the game.

Kappa Road itself clears without either item; only its route onward to Naruto
Road needs them, so that one check carries the rule rather than the level.

Not gates, though the walkthroughs mention the abilities there: the star blocks
in Lost'n Road and Tomb Pass hide golden fortune dolls, and Sky Garden's dive
leads somewhere optional. None of them is an entry pass.

Yae is in Otohime Town, inside Ryugu, so Mafu Island, Underworld and Floating
Castle all depend on Ryugu having been opened.

## Locations

51 locations, each an entry pass, a castle clear or a character joining, keyed
on its event flag bit:

```
code = (guest_flag_byte - 0x8008821C) * 8 + bit_index
```

so the mod reports a check straight from the flag it saw change, with no table
of its own.

Two sit outside the 160 bit flag block the mod diffs. Yae's location keys on pak
`0x623` bit 3, which is inside it and fires at the same moment as her unlock
bit, so it needs nothing special. **Sasuke has no equivalent**: his only flag is
the unlock bit at pak `0x70F`, giving code 1951, so the mod has to watch that
byte as well as the block or the check never fires.

## Fill

A greedy max-gain simulation places every progression item and reaches all 51
locations, never holding more items than there are open locations.

Edo is the tight spot. Five locations are open before Chain Pipe is found, since
Ringbell Pass is behind it, against a checkpoint wanting five entry passes. It
fits, but Chain Pipe cannot be placed on `Ringbell Pass: Stage Clear`, and there
is little room for filler, so a multiworld will put few other players' items in
Edo.

## Unverified

- whether Sasuke gates anything. He is a location, not an item.
- whether any level needs an ability to *clear* rather than to enter. Only entry
  is modelled.
- the worlds past Edo, which were never walked in game. Their level lists come
  from the save editor template.
