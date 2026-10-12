from dataclasses import dataclass

from Options import DeathLink, PerGameCommonOptions


@dataclass
class GGAOptions(PerGameCommonOptions):
    death_link: DeathLink
