from typing import Dict, NamedTuple, Optional

from BaseClasses import Item, ItemClassification


class GGAItem(Item):
    game = "Goemon's Great Adventure Recompiled"


class GGAItemData(NamedTuple):
    code: Optional[int] = None
    type: ItemClassification = ItemClassification.filler
    num_exist: int = 1


# Codes must fit in 32 bits: the glue's hasItem compares the full Archipelago
# item code against a u32 passed from the mod, so anything wider never matches.
#
# 44 entry passes exist in the game, by the cumulative cap table at
# D_8010AEB8_5C60D8. One is placed per pass location, and the checkpoints
# consume them as a counted requirement rather than individually.
item_data_table: Dict[str, GGAItemData] = {
    "Entry Pass": GGAItemData(
        code=1,
        type=ItemClassification.progression,
        num_exist=44,
    ),
    "Ryo": GGAItemData(
        code=2,
        type=ItemClassification.filler,
    ),
    "Chain Pipe": GGAItemData(
        code=3,
        type=ItemClassification.progression,
    ),
    # Joining up in Otohime Town is what lets the party cross water, so this is
    # the swim gate. The same event teaches Sasuke to swim.
    "Yae": GGAItemData(
        code=4,
        type=ItemClassification.progression,
    ),
    # Each castle's clear is what opens the next world, so the opening is an
    # item rather than an implicit consequence of beating the boss.
    # A world's access item opens every level in it except the castle, which
    # stays behind its checkpoint's entry pass count. Edo needs none; the player
    # starts there.
    "Ryugu Access": GGAItemData(code=5, type=ItemClassification.progression),
    "Mafu Island Access": GGAItemData(code=6, type=ItemClassification.progression),
    "Underworld Access": GGAItemData(code=7, type=ItemClassification.progression),
    "Floating Castle Access": GGAItemData(code=8, type=ItemClassification.progression),
}

item_table = {name: data.code for name, data in item_data_table.items() if data.code is not None}

code_to_item_table = {data.code: name for name, data in item_data_table.items() if data.code is not None}
