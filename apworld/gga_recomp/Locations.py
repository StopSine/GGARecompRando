from typing import Dict, NamedTuple, Optional

from BaseClasses import Location


class GGALocation(Location):
    game = "Goemon's Great Adventure Recompiled"


class GGALocationData(NamedTuple):
    region: str
    code: Optional[int] = None


# A code is the event flag's own bit index, so the mod can report a check
# straight from the flag it saw change, with no table of its own:
#
#   code = (guest_flag_byte - 0x8008821C) * 8 + bit_index
#
# Save editor byte 0x11c is pak 0x61C, guest 0x8008821C, so the byte number is
# (template_byte - 0x11c). Names follow the save editor template's labels; see
# lib/gga/docs/save_memory_map.md in the Goemon64Recomp repo.
def _code(byte: int, bit: int) -> int:
    return (byte - 0x11c) * 8 + bit


location_data_table: Dict[str, GGALocationData] = {
    # Edo
    "Lost'n Town: Beat Mania": GGALocationData("Lost'n Town", _code(0x11c, 6)),
    "Lost'n Town: Tomezo": GGALocationData("Lost'n Town", _code(0x11c, 7)),
    "Lost'n Town: Iguana Man": GGALocationData("Lost'n Town", _code(0x122, 3)),
    "Lost'n Road: Stage Clear": GGALocationData("Lost'n Road", _code(0x11d, 0)),
    "Digadig Gold Mine: Stage Clear": GGALocationData("Digadig Gold Mine", _code(0x11d, 1)),
    "Ringbell Pass: Stage Clear": GGALocationData("Ringbell Pass", _code(0x11d, 2)),

    # Ryugu
    "Kappa Road: Large Octopus": GGALocationData("Kappa Road", _code(0x11d, 4)),
    "Naruto Road: Large Octopus": GGALocationData("Naruto Road", _code(0x11d, 5)),
    "Kappa Road: Stage Clear": GGALocationData("Kappa Road", _code(0x11d, 6)),
    "Frog Mountain: Stage Clear": GGALocationData("Frog Mountain", _code(0x11d, 7)),
    "Naruto Road: Stage Clear": GGALocationData("Naruto Road", _code(0x11e, 0)),
    "Kappa Road: Stage Clear to Naruto Road": GGALocationData("Kappa Road", _code(0x11e, 1)),
    "Frog Mountain: Iguana Man": GGALocationData("Frog Mountain", _code(0x11e, 2)),
    "Otohime Town: Obisumaru": GGALocationData("Otohime Town", _code(0x11d, 3)),
    "Otohime Town: Waterfall": GGALocationData("Otohime Town", _code(0x122, 4)),

    # Mafu Island
    "Spook Village: Roadside Guardian": GGALocationData("Spook Village", _code(0x11e, 3)),
    "Spook Village: Old Woman": GGALocationData("Spook Village", _code(0x11e, 4)),
    "Spook Village: Fireworks Boss": GGALocationData("Spook Village", _code(0x11e, 5)),
    "Spook Village: Iguana Men": GGALocationData("Spook Village", _code(0x11e, 6)),
    "Kokeshi Road: Stage Clear": GGALocationData("Kokeshi Road", _code(0x11e, 7)),
    "Kokeshi Road: Stage Clear to Pochan Lake": GGALocationData("Kokeshi Road", _code(0x11f, 0)),
    "Pochan Lake: Stage Clear to Spook Village": GGALocationData("Pochan Lake", _code(0x11f, 1)),
    "Pochan Lake: Stage Clear": GGALocationData("Pochan Lake", _code(0x11f, 2)),
    "Kokeshi Road: Stage Clear to Tomb Pass": GGALocationData("Kokeshi Road", _code(0x11f, 3)),
    "Spook Village: Stage Clear to Mafu Checkpoint": GGALocationData("Spook Village", _code(0x123, 1)),

    # Underworld
    "Tree Stump Village: Hide and Seek": GGALocationData("Tree Stump Village", _code(0x11f, 4)),
    "Tree Stump Village: Okiku": GGALocationData("Tree Stump Village", _code(0x11f, 5)),
    "Tree Stump Village: Girl": GGALocationData("Tree Stump Village", _code(0x11f, 6)),
    "Tree Stump Village: Kensuke": GGALocationData("Tree Stump Village", _code(0x11f, 7)),
    "Kaiware Road: Stage Clear": GGALocationData("Kaiware Road", _code(0x120, 0)),
    "Mokeke Forest: Stage Clear": GGALocationData("Mokeke Forest", _code(0x120, 1)),
    "Wonder Valley: Stage Clear to Tree Stump Village": GGALocationData("Wonder Valley", _code(0x120, 2)),
    "Creep Village: Admirative Tengu": GGALocationData("Creep Village", _code(0x122, 7)),
    "Creep Village: Hungry Tengu": GGALocationData("Creep Village", _code(0x123, 0)),
    "Tree Stump Village: Stage Clear to Underworld Checkpoint": GGALocationData(
        "Tree Stump Village", _code(0x123, 2)),

    # Floating Castle
    "Floating World Town: Scarfman 1": GGALocationData("Floating World Town", _code(0x120, 3)),
    "Floating World Town: Obisumaru": GGALocationData("Floating World Town", _code(0x120, 4)),
    "Floating World Town: Tagosaku": GGALocationData("Floating World Town", _code(0x120, 5)),
    "Floating World Town: Omatsu": GGALocationData("Floating World Town", _code(0x120, 6)),
    "Floating World Town: Scarfman 2": GGALocationData("Floating World Town", _code(0x120, 7)),
    "Cloud Fortress: Stage Clear to Wind Castle Wall": GGALocationData("Cloud Fortress", _code(0x121, 0)),
    "Sky Garden: Stage Clear": GGALocationData("Sky Garden", _code(0x121, 1)),
    "Wind Castle Wall: Stage Clear": GGALocationData("Wind Castle Wall", _code(0x121, 2)),
    "Cloud Fortress: Stage Clear to Sky Garden": GGALocationData("Cloud Fortress", _code(0x121, 3)),

    # Castles, which gate the worlds
    "Edo Castle: Cleared": GGALocationData("Edo Castle", _code(0x121, 5)),
    "Ryugu Castle: Cleared": GGALocationData("Ryugu Castle", _code(0x121, 6)),
    "Majin Castle: Cleared": GGALocationData("Majin Castle", _code(0x121, 7)),
    "Underworld Castle: Cleared": GGALocationData("Underworld Castle", _code(0x122, 0)),

    # The Chain Pipe is 100 ryo from Kenzo in Lost'n Town.
    #
    # Yae joins in Otohime Town, which is what lets the party cross water.
    # Joining sets this bit and the Yae unlocked bit at template 0x212 together;
    # this one is used because only it falls inside the 160 bit flag block the
    # mod diffs.
    "Lost'n Town: Chain Pipe from Kenzo": GGALocationData("Lost'n Town", _code(0x122, 2)),
    "Otohime Town: Yae Joins": GGALocationData("Otohime Town", _code(0x123, 3)),

    # Sasuke has no flag inside the 160 bit block, only his own unlock bit, so
    # the mod has to watch this byte as well as the block.
    "Lost'n Town: Sasuke Joins": GGALocationData("Lost'n Town", _code(0x20f, 7)),
}

location_table = {name: data.code for name, data in location_data_table.items() if data.code is not None}

code_to_location_table = {data.code: name for name, data in location_data_table.items() if data.code is not None}

locked_locations: Dict[str, str] = {}
