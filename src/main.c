#include "gga_ap.h"
#include "recompui.h"
#include "recomputils.h"

bool apConnected = false;
extern bool apConnectPending;

static bool uiCreated = false;

static u32 lastItemsSize = 0;

static u8 flagSnapshot[AP_FLAG_BLOCK_SIZE];
static u8 sasukeSnapshot;
static u8 passSnapshot[AP_PASS_COUNT_WORLDS];
static bool snapshotTaken = false;
static bool goalSent = false;

// randoItemGive raises the pass counts, and the rollback below would undo that,
// so the snapshot has to follow anything Archipelago grants.
static void syncPassSnapshot(void) {
    for (u32 i = 0; i < AP_PASS_COUNT_WORLDS; i++) {
        passSnapshot[i] = *(volatile u8*)(AP_PASS_COUNT_BASE + i);
    }
}

static void randoInitUi(void) {
    randoCreateNotificationContainer();
    randoCreateAPConnectMenu();
    randoShowAPConnectMenu();
    uiCreated = true;
}

static void pollConnection(void) {
    if (!apConnectPending) {
        return;
    }

    if (rando_is_connected()) {
        apConnectPending = false;
        apConnected = true;

        char slot_name[64];
        rando_get_own_slot_name(slot_name);
        randoEmitNormalNotification("Connected");

        // Without this the server never sends what each location holds, so a
        // send notification can only name the location.
        rando_queue_scouts_all();
        rando_send_queued_scouts(0);

        lastItemsSize = rando_get_items_size();
    }
    else if (rando_connect_failed()) {
        apConnectPending = false;
        randoEmitErrorNotification("Failed to connect");
        randoShowAPConnectMenu();
    }
}

// AP_ItemType is FILLER, PROGRESSION, USEFUL, TRAP; the notification enum is
// PROGRESSION, USEFUL, JUNK, TRAP, so the two cannot be cast across.
static RandoItemClassification classificationFromApType(u32 ap_type) {
    switch (ap_type) {
        case 1:  return RANDO_ITEM_CLASS_PROGRESSION;
        case 2:  return RANDO_ITEM_CLASS_USEFUL;
        case 3:  return RANDO_ITEM_CLASS_TRAP;
        default: return RANDO_ITEM_CLASS_JUNK;
    }
}

// The item behind a location is only known once the scouts come back, so fall
// back to naming the location when they have not.
static void randoAnnounceSend(u32 location_id) {
    char item_name[128];
    char player_name[64];

    item_name[0] = 0;
    player_name[0] = 0;

    if (rando_is_scouted()) {
        rando_get_location_item_name(location_id, item_name);
        rando_get_location_item_player(location_id, player_name);
    }

    if (item_name[0] == 0 || player_name[0] == 0) {
        randoEmitNormalNotification("Sent a check");
        return;
    }

    randoEmitSendNotification(item_name, player_name,
                              classificationFromApType(rando_get_location_type(location_id)));
}

static void pollItems(void) {
    u32 items_size = rando_get_items_size();

    if (items_size <= lastItemsSize) {
        return;
    }

    for (u32 i = lastItemsSize; i < items_size; i++) {
        u32 item_id = rando_get_item(i);

        char item_name[128];
        char sender_name[64];
        rando_get_item_name_from_id(item_id, item_name);
        rando_get_sending_player_name(i, sender_name);

        randoItemGive(item_id);
        randoEmitReceiveNotification(item_name, sender_name, RANDO_ITEM_CLASS_PROGRESSION);
    }

    syncPassSnapshot();
    lastItemsSize = items_size;
}

RECOMP_HOOK("game_frame_update")
void randoTick(void) {
    if (!uiCreated) {
        randoInitUi();
    }

    recomp_run_ui_callbacks();
    notificationUpdateCycle();
    pollConnection();

    if (apConnected) {
        pollItems();
        randoPollChecks();
    }
}

// Flags that are the reward rather than a record of one. The game grants these
// itself, so once the check is reported the bit has to be cleared again or the
// player keeps the vanilla unlock as well as whatever Archipelago sends.
static const u32 rewardFlagBytes[] = {
    0x80088222,  // bit 2 chain pipe
    0x80088223,  // bit 3 yae and the swim unlock
};
static const u32 rewardFlagBits[] = { 2, 3 };
#define REWARD_FLAG_COUNT 2

static void clearRewardFlag(u32 location_id) {
    for (u32 i = 0; i < REWARD_FLAG_COUNT; i++) {
        u32 id = (rewardFlagBytes[i] - AP_FLAG_BLOCK) * 8 + rewardFlagBits[i];
        if (id == location_id) {
            volatile u8* p = (volatile u8*)rewardFlagBytes[i];
            *p = (u8)(*p & ~(1 << rewardFlagBits[i]));
            return;
        }
    }

    if (location_id == (AP_SASUKE_BYTE - AP_FLAG_BLOCK) * 8 + AP_SASUKE_BIT) {
        volatile u8* p = (volatile u8*)AP_SASUKE_BYTE;
        *p = (u8)(*p & ~(1 << AP_SASUKE_BIT));
    }
}

static void sendCheck(u32 location_id) {
    clearRewardFlag(location_id);

    if (rando_location_is_checked(location_id)) {
        return;
    }
    rando_send_location(location_id);
    randoAnnounceSend(location_id);
}

// The game writes these flags itself, so a check is whatever bit it just set
// rather than anything the mod has to be told about.
void randoPollChecks(void) {
    u8 current[AP_FLAG_BLOCK_SIZE];

    for (u32 i = 0; i < AP_FLAG_BLOCK_SIZE; i++) {
        current[i] = *(volatile u8*)(AP_FLAG_BLOCK + i);
    }
    u8 sasuke = *(volatile u8*)AP_SASUKE_BYTE;

    if (!snapshotTaken) {
        for (u32 i = 0; i < AP_FLAG_BLOCK_SIZE; i++) {
            flagSnapshot[i] = current[i];
        }
        sasukeSnapshot = sasuke;
        syncPassSnapshot();
        snapshotTaken = true;
        return;
    }

    for (u32 i = 0; i < AP_FLAG_BLOCK_SIZE; i++) {
        u8 newly_set = (u8)(current[i] & ~flagSnapshot[i]);
        flagSnapshot[i] = current[i];

        for (u32 bit = 0; bit < 8; bit++) {
            if (newly_set & (1 << bit)) {
                sendCheck(i * 8 + bit);
            }
        }
    }

    u8 sasuke_set = (u8)(sasuke & ~sasukeSnapshot);
    sasukeSnapshot = sasuke;
    if (sasuke_set & (1 << AP_SASUKE_BIT)) {
        sendCheck((AP_SASUKE_BYTE - AP_FLAG_BLOCK) * 8 + AP_SASUKE_BIT);
    }

    if (!goalSent &&
        *(volatile u8*)AP_DREAM_CASTLE_PROGRESS >= AP_CASTLE_PROGRESS_DONE) {
        rando_complete_goal();
        goalSent = true;
        randoEmitNormalNotification("Goal complete");
    }

    // The game awards the pass itself. Archipelago decides who actually gets
    // one, so any increase the game makes is rolled back; randoItemGive is the
    // only thing allowed to raise these.
    for (u32 i = 0; i < AP_PASS_COUNT_WORLDS; i++) {
        volatile u8* p = (volatile u8*)(AP_PASS_COUNT_BASE + i);
        if (*p > passSnapshot[i]) {
            *p = passSnapshot[i];
        }
        else {
            passSnapshot[i] = *p;
        }
    }
}
