#!/usr/bin/env python3
"""Zip the world package into gga_recomp.apworld for an Archipelago install."""

import pathlib
import zipfile

SRC = pathlib.Path(__file__).parent / "gga_recomp"
OUT = pathlib.Path(__file__).parent / "gga_recomp.apworld"
TMP = OUT.with_suffix(".apworld.tmp")


def main() -> int:
    # Written to one side and moved into place, because Archipelago reading the
    # file while it is still being written reports it as a truncated stream.
    with zipfile.ZipFile(TMP, "w", zipfile.ZIP_DEFLATED) as z:
        for path in sorted(SRC.rglob("*.py")):
            z.write(path, path.relative_to(SRC.parent).as_posix())

    with zipfile.ZipFile(TMP) as z:
        bad = z.testzip()
        if bad is not None:
            print(f"bad member: {bad}")
            return 1
        names = z.namelist()

    TMP.replace(OUT)

    print(f"{OUT.name}: {OUT.stat().st_size} bytes")
    for name in names:
        print(f"  {name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
