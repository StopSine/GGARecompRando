#include "gga_ap.h"
#include "recompui.h"
#include "recomputils.h"
#include "recompconfig.h"

bool apConnected = false;
extern bool apConnectPending;

static bool uiCreated = false;

static u32 lastItemsSize = 0;

static u8 flagSnapshot[AP_FLAG_BLOCK_SIZE];
static u8 sasukeSnapshot;
static u8 passSnapshot[AP_PASS_COUNT_WORLDS];
static bool snapshotTaken = false;
static bool goalSent = false;

// Must follow randoItemGive, or the rollback in randoPollChecks undoes it.
static void syncPassSnapshot(void) {
    for (u32 i = 0; i < AP_PASS_COUNT_WORLDS; i++) {
        passSnapshot[i] = *(volatile u8*)(AP_PASS_COUNT_BASE + i);
    }
}

// Likewise for the flags randoItemGive sets: without this the next poll reads
// the grant as a freshly set check and clearRewardFlag wipes it the same frame.
static void syncRewardFlagSnapshot(void);

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
        randoHideAPConnectMenu();
        randoEmitNormalNotification("Connected");

        // Without this the server never says what each location holds.
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

// AP_ItemType is FILLER, PROGRESSION, USEFUL, TRAP; the two enums do not align.
static RandoItemClassification classificationFromApType(u32 ap_type) {
    switch (ap_type) {
        case 1:  return RANDO_ITEM_CLASS_PROGRESSION;
        case 2:  return RANDO_ITEM_CLASS_USEFUL;
        case 3:  return RANDO_ITEM_CLASS_TRAP;
        default: return RANDO_ITEM_CLASS_JUNK;
    }
}

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
    syncRewardFlagSnapshot();
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
        randoApplyWorldAccess();
        randoPollChecks();
        randoPollDeathLink();
    }
}

static u8 healthSnapshot = 0;
static bool healthSnapshotTaken = false;
// An incoming death zeroes health, which would otherwise bounce straight back out.
static bool deathFromLink = false;

// The slot decides whether death link exists; this only lets the player opt out.
static bool deathLinkOn(void) {
    return rando_get_death_link_enabled() && recomp_get_config_u32("death_link") == 1;
}

RECOMP_HOOK_RETURN("func_80189D78_5EC7D8")
void randoHealthWritten(void) {
    if (!apConnected) {
        return;
    }

    u8 health = *(volatile u8*)AP_P1_HEALTH;

    if (!deathLinkOn()) {
        healthSnapshot = health;
        healthSnapshotTaken = true;
        return;
    }

    if (health == 0 && healthSnapshot > 0) {
        if (deathFromLink) {
            deathFromLink = false;
        }
        else {
            rando_send_death_link();
        }
    }

    healthSnapshot = health;
    healthSnapshotTaken = true;
}

void randoPollDeathLink(void) {
    // Dropped rather than queued, so switching the toggle back on cannot fire a
    // death that arrived while it was off.
    if (!deathLinkOn()) {
        if (rando_get_death_link_pending()) {
            rando_reset_death_link_pending();
        }
        return;
    }

    if (!healthSnapshotTaken) {
        healthSnapshot = *(volatile u8*)AP_P1_HEALTH;
        healthSnapshotTaken = true;
    }

    if (rando_get_death_link_pending()) {
        rando_reset_death_link_pending();
        if (*(volatile u8*)AP_P1_HEALTH > 0) {
            *(volatile u8*)AP_P1_HEALTH = 0;
            deathFromLink = true;
            healthSnapshot = 0;
            randoEmitNormalNotification("Death link received");
        }
    }
}

// Flags that are the reward rather than a record of one. Cleared after reporting,
// or the player keeps the vanilla unlock as well as what Archipelago sends.
static const u32 rewardFlagBytes[] = {
    0x8008822A,  // bit 2 chain pipe
    0x8008822B,  // bit 3 yae and the swim unlock
};
static const u32 rewardFlagBits[] = { 2, 3 };
#define REWARD_FLAG_COUNT 2

static void syncRewardFlagSnapshot(void) {
    for (u32 i = 0; i < REWARD_FLAG_COUNT; i++) {
        u32 index = rewardFlagBytes[i] - AP_FLAG_BLOCK;
        flagSnapshot[index] = *(volatile u8*)rewardFlagBytes[i];
    }
}

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

    // The game awards passes itself, so its increases are rolled back and only
    // randoItemGive may raise these.
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
