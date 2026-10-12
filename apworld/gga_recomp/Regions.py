from typing import Dict, List, NamedTuple


class GGARegionData(NamedTuple):
    connecting_regions: List[str] = []


# A world is reached by holding its access item, not by clearing the world
# before it, so the five worlds hang off Menu rather than off each other. Edo is
# free because the player starts there.
#
# Within a world the levels hang off a hub, because the access item opens all of
# them at once. Only the gates that are real in game sit between a hub and a
# level: the Chain Pipe for Ringbell Pass and Yae for the two water routes. The
# castle is behind the checkpoint and its entry pass requirement.
region_data_table: Dict[str, GGARegionData] = {
    "Menu": GGARegionData([
        "Edo", "Ryugu", "Mafu Island", "Underworld", "Floating Castle",
    ]),

    "Edo": GGARegionData([
        "Home of the Wiseman", "Lost'n Road", "Lost'n Town",
        "Ringbell Pass", "Digadig Gold Mine", "Edo Checkpoint",
    ]),
    "Home of the Wiseman": GGARegionData([]),
    "Lost'n Road": GGARegionData([]),
    "Lost'n Town": GGARegionData([]),
    "Ringbell Pass": GGARegionData([]),
    "Digadig Gold Mine": GGARegionData([]),
    "Edo Checkpoint": GGARegionData(["Edo Castle"]),
    "Edo Castle": GGARegionData([]),

    "Ryugu": GGARegionData([
        "Kappa Road", "Naruto Road", "Frog Mountain", "Otohime Town",
        "Ryugu Checkpoint",
    ]),
    "Kappa Road": GGARegionData([]),
    "Naruto Road": GGARegionData([]),
    "Frog Mountain": GGARegionData([]),
    "Otohime Town": GGARegionData([]),
    "Ryugu Checkpoint": GGARegionData(["Ryugu Castle"]),
    "Ryugu Castle": GGARegionData([]),

    "Mafu Island": GGARegionData([
        "Kokeshi Road", "Maneater Road", "Pochan Lake", "Tomb Pass",
        "Spook Village", "Mafu Checkpoint",
    ]),
    "Kokeshi Road": GGARegionData([]),
    "Maneater Road": GGARegionData([]),
    "Pochan Lake": GGARegionData([]),
    "Tomb Pass": GGARegionData([]),
    "Spook Village": GGARegionData([]),
    "Mafu Checkpoint": GGARegionData(["Majin Castle"]),
    "Majin Castle": GGARegionData([]),

    "Underworld": GGARegionData([
        "Kaiware Road", "Mokeke Forest", "Wonder Valley",
        "Tree Stump Village", "Creep Village", "Underworld Checkpoint",
    ]),
    "Kaiware Road": GGARegionData([]),
    "Mokeke Forest": GGARegionData([]),
    "Wonder Valley": GGARegionData([]),
    "Tree Stump Village": GGARegionData([]),
    "Creep Village": GGARegionData([]),
    "Underworld Checkpoint": GGARegionData(["Underworld Castle"]),
    "Underworld Castle": GGARegionData([]),

    "Floating Castle": GGARegionData([
        "Cloud Fortress", "Sky Garden", "Wind Castle Wall",
        "Floating World Town", "Floating Checkpoint",
    ]),
    "Cloud Fortress": GGARegionData([]),
    "Sky Garden": GGARegionData([]),
    "Wind Castle Wall": GGARegionData([]),
    "Floating World Town": GGARegionData([]),
    "Floating Checkpoint": GGARegionData(["Dream Castle"]),
    "Dream Castle": GGARegionData([]),
}

# The access item each world needs. Edo has none; the player starts there.
world_access_items: Dict[str, str] = {
    "Ryugu": "Ryugu Access",
    "Mafu Island": "Mafu Island Access",
    "Underworld": "Underworld Access",
    "Floating Castle": "Floating Castle Access",
}

# Entry passes a checkpoint wants before it opens, five more each world.
checkpoint_pass_requirements: Dict[str, int] = {
    "Edo Checkpoint": 5,
    "Ryugu Checkpoint": 10,
    "Mafu Checkpoint": 15,
    "Underworld Checkpoint": 20,
    "Floating Checkpoint": 25,
}

# Gates inside a world that the access item does not cover, all of them needed
# together. Creep Village is reached by diving through Mokeke Forest, both of
# these castles have stretches that have to be swum, and the route from Kappa
# Road to Naruto Road dives and then needs the chain pipe for its star blocks.
level_requirements: Dict[str, List[str]] = {
    "Ringbell Pass": ["Chain Pipe"],
    "Kokeshi Road": ["Chain Pipe"],
    "Naruto Road": ["Yae", "Chain Pipe"],
    "Pochan Lake": ["Yae"],
    "Creep Village": ["Yae"],
    "Ryugu Castle": ["Yae"],
    "Dream Castle": ["Yae"],
}

# A single check behind a gate its level is not. Kappa Road is cleared without
# either item; only the route onward to Naruto Road needs them.
location_requirements: Dict[str, List[str]] = {
    "Kappa Road: Stage Clear to Naruto Road": ["Yae", "Chain Pipe"],
}


def get_exit(region: str, target: str) -> str:
    return f"{region} -> {target}"
