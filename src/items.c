#include "gga_ap.h"

// From D_8010AEC0_5C60E0, what the game's own award function clamps against.
static const u8 pass_caps[AP_PASS_COUNT_WORLDS] = { 6, 8, 10, 8, 9, 3 };

static void setFlagBit(u32 guest_byte, u32 bit) {
    volatile u8* p = (volatile u8*)guest_byte;
    *p = (u8)(*p | (1 << bit));
}

// Bit 0 reveals the node, bit 1 is the map walking the route open.
static void openNodes(u32 first_guest_byte, u32 count) {
    for (u32 i = 0; i < count; i++) {
        volatile u8* p = (volatile u8*)(first_guest_byte + i);
        *p = (u8)(*p | 0x03);
    }
}

// Each group stops one short of the castle's own access byte, which stays shut so
// the castle remains behind its checkpoint's entry pass count. Mafu runs one
// longer because Spook Village is reachable from two directions.
#define EDO_ACCESS_FIRST 0x800882AA
#define EDO_ACCESS_COUNT 6
#define RYUGU_ACCESS_FIRST 0x800882BA
#define RYUGU_ACCESS_COUNT 5
#define MAFU_ACCESS_FIRST 0x800882CA
#define MAFU_ACCESS_COUNT 6
#define UNDERWORLD_ACCESS_FIRST 0x800882DA
#define UNDERWORLD_ACCESS_COUNT 5
#define FLOATING_ACCESS_FIRST 0x800882EA
#define FLOATING_ACCESS_COUNT 5

// Reapplied every frame: loading or starting a save puts these bytes back. Edo
// has no access item, the player starts there.
void randoApplyWorldAccess(void) {
    openNodes(EDO_ACCESS_FIRST, EDO_ACCESS_COUNT);

    if (rando_has_item(AP_ITEM_RYUGU_ACCESS)) {
        openNodes(RYUGU_ACCESS_FIRST, RYUGU_ACCESS_COUNT);
    }
    if (rando_has_item(AP_ITEM_MAFU_ACCESS)) {
        openNodes(MAFU_ACCESS_FIRST, MAFU_ACCESS_COUNT);
    }
    if (rando_has_item(AP_ITEM_UNDERWORLD_ACCESS)) {
        openNodes(UNDERWORLD_ACCESS_FIRST, UNDERWORLD_ACCESS_COUNT);
    }
    if (rando_has_item(AP_ITEM_FLOATING_ACCESS)) {
        openNodes(FLOATING_ACCESS_FIRST, FLOATING_ACCESS_COUNT);
    }
}

static void giveEntryPass(void) {
    // Counts are per world, so top up the first with room rather than tracking
    // which world this pass was for.
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
            setFlagBit(0x8008822A, 2);
            break;

        case AP_ITEM_YAE:
            setFlagBit(0x8008822B, 3);
            setFlagBit(0x8008831A, 7);
            break;

        // World access is applied by randoApplyWorldAccess from what is held.
        default:
            break;
    }
}
