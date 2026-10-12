#include "gga_ap.h"

// Caps per world from D_8010AEC0_5C60E0, which the game's own award function
// clamps against.
static const u8 pass_caps[AP_PASS_COUNT_WORLDS] = { 6, 8, 10, 8, 9, 3 };

static void setFlagBit(u32 guest_byte, u32 bit) {
    volatile u8* p = (volatile u8*)guest_byte;
    *p = (u8)(*p | (1 << bit));
}

// A world is opened by setting bit 0 and bit 1 on its access bytes, which is
// what the game does itself in two steps: reveal, then the map walking it open.
static void openNodes(u32 first_guest_byte, u32 count) {
    for (u32 i = 0; i < count; i++) {
        volatile u8* p = (volatile u8*)(first_guest_byte + i);
        *p = (u8)(*p | 0x03);
    }
}

// Access bytes per world, the group that gates travel, from the save editor
// template's 0x1b2, 0x1c2, 0x1d2 and 0x1e2 plus 0x88108. Mafu has seven because
// Spook Village is reachable from two directions. Edo is not here; the player
// starts with it.
#define RYUGU_ACCESS_FIRST 0x800882BA
#define RYUGU_ACCESS_COUNT 6
#define MAFU_ACCESS_FIRST 0x800882CA
#define MAFU_ACCESS_COUNT 7
#define UNDERWORLD_ACCESS_FIRST 0x800882DA
#define UNDERWORLD_ACCESS_COUNT 6
#define FLOATING_ACCESS_FIRST 0x800882EA
#define FLOATING_ACCESS_COUNT 6

static void giveEntryPass(void) {
    // The count is per world and the game clamps it, so top up the first world
    // that still has room rather than tracking which pass this was.
    for (u32 i = 0; i < AP_PASS_COUNT_WORLDS; i++) {
        volatile u8* p = (volatile u8*)(AP_PASS_COUNT_BASE + i);
        if (*p < pass_caps[i]) {
            *p = (u8)(*p + 1);
            return;
        }
    }
}

void randoItemGive(u32 ap_item_id) {
    switch (ap_item_id) {
        case AP_ITEM_ENTRY_PASS:
            giveEntryPass();
            break;

        case AP_ITEM_CHAIN_PIPE:
            setFlagBit(0x80088222, 2);
            break;

        case AP_ITEM_YAE:
            setFlagBit(0x80088223, 3);
            setFlagBit(0x8008831A, 7);
            break;

        case AP_ITEM_RYUGU_ACCESS:
            openNodes(RYUGU_ACCESS_FIRST, RYUGU_ACCESS_COUNT);
            break;

        case AP_ITEM_MAFU_ACCESS:
            openNodes(MAFU_ACCESS_FIRST, MAFU_ACCESS_COUNT);
            break;

        case AP_ITEM_UNDERWORLD_ACCESS:
            openNodes(UNDERWORLD_ACCESS_FIRST, UNDERWORLD_ACCESS_COUNT);
            break;

        case AP_ITEM_FLOATING_ACCESS:
            openNodes(FLOATING_ACCESS_FIRST, FLOATING_ACCESS_COUNT);
            break;

        default:
            break;
    }
}
