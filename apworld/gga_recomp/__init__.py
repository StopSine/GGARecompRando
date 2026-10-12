from BaseClasses import Region, Tutorial, ItemClassification
from worlds.AutoWorld import WebWorld, World

from .Items import GGAItem, item_data_table, item_table
from .Locations import GGALocation, location_data_table, location_table
from .Options import GGAOptions
from .Regions import (
    checkpoint_pass_requirements,
    get_exit,
    level_requirements,
    location_requirements,
    region_data_table,
    world_access_items,
)


class GGAWebWorld(WebWorld):
    setup_en = Tutorial(
        tutorial_name="Start Guide",
        description="A guide to playing Goemon's Great Adventure Recompiled in Archipelago.",
        language="English",
        file_name="guide_en.md",
        link="guide/en",
        authors=["StopSine"],
    )

    tutorials = [setup_en]


class GGAWorld(World):
    """Goemon's Great Adventure."""

    game = "Goemon's Great Adventure Recompiled"
    web = GGAWebWorld()
    options_dataclass = GGAOptions
    options: GGAOptions
    location_name_to_id = location_table
    item_name_to_id = item_table

    # The glue reads death_link out of slot data, so it has to be published
    # here, not only set in the yaml.
    def fill_slot_data(self) -> dict:
        return {"death_link": int(self.options.death_link.value)}

    def create_item(self, name: str) -> GGAItem:
        data = item_data_table[name]
        return GGAItem(name, data.type, data.code, self.player)

    def create_regions(self) -> None:
        for region_name in region_data_table:
            self.multiworld.regions.append(Region(region_name, self.player, self.multiworld))

        for region_name, region_data in region_data_table.items():
            region = self.multiworld.get_region(region_name, self.player)
            region.add_locations({
                name: data.code
                for name, data in location_data_table.items()
                if data.region == region_name
            }, GGALocation)
            region.add_exits(region_data.connecting_regions)

    def create_items(self) -> None:
        total_locations = len(self.multiworld.get_unfilled_locations(self.player))

        # Unique progression first. Entry passes fill whatever room is left, so
        # trimming the pool to fit can never discard a key or an ability.
        item_pool = [
            self.create_item(name)
            for name, data in item_data_table.items()
            if name not in ("Ryo", "Entry Pass")
        ]

        passes = item_data_table["Entry Pass"].num_exist
        room = total_locations - len(item_pool)
        item_pool += [self.create_item("Entry Pass") for _ in range(min(passes, max(room, 0)))]

        while len(item_pool) < total_locations:
            item_pool.append(self.create_item("Ryo"))
        del item_pool[total_locations:]

        self.multiworld.itempool += item_pool

    def set_rules(self) -> None:
        player = self.player
        multiworld = self.multiworld

        def entrances_into(target):
            for source, data in region_data_table.items():
                if target in data.connecting_regions:
                    yield multiworld.get_entrance(get_exit(source, target), player)

        # A checkpoint wants a number of entry passes before it opens.
        for checkpoint, needed in checkpoint_pass_requirements.items():
            for entrance in entrances_into(checkpoint):
                entrance.access_rule = (
                    lambda state, n=needed: state.has("Entry Pass", player, n)
                )

        # A world's access item opens the whole world. Edo has none.
        for world, item in world_access_items.items():
            for entrance in entrances_into(world):
                entrance.access_rule = lambda state, i=item: state.has(i, player)

        # Gates inside a world the access item does not cover.
        for level, items in level_requirements.items():
            for entrance in entrances_into(level):
                entrance.access_rule = (
                    lambda state, n=items: state.has_all(n, player)
                )

        for name, items in location_requirements.items():
            location = multiworld.get_location(name, player)
            location.access_rule = lambda state, n=items: state.has_all(n, player)

        # The game ends at Dream Castle and the save has no flag for clearing it,
        # so completion is reaching it rather than a check firing.
        multiworld.completion_condition[player] = (
            lambda state: state.can_reach("Dream Castle", "Region", player)
        )
