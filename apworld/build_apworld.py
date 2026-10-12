#!/usr/bin/env python3
"""Zip the world package into gga_recomp.apworld for an Archipelago install."""

import pathlib
import zipfile

SRC = pathlib.Path(__file__).parent / "gga_recomp"
OUT = pathlib.Path(__file__).parent / "gga_recomp.apworld"


def main() -> int:
    with zipfile.ZipFile(OUT, "w", zipfile.ZIP_DEFLATED) as z:
        for path in sorted(SRC.rglob("*.py")):
            z.write(path, path.relative_to(SRC.parent).as_posix())
    print(f"{OUT.name}: {OUT.stat().st_size} bytes")
    for name in zipfile.ZipFile(OUT).namelist():
        print(f"  {name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
