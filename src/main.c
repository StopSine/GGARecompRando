#include "gga_ap.h"
#include "recompui.h"
#include "recomputils.h"

bool apConnected = false;
extern bool apConnectPending;

static bool uiCreated = false;

static u32 lastItemsSize = 0;

static bool entryPassSent = false;

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
    }
}

void randoEntryPassCollected(void) {
    if (!apConnected || entryPassSent) {
        return;
    }

    rando_send_location(AP_LOC_ENTRY_PASS);
    entryPassSent = true;
    randoAnnounceSend(AP_LOC_ENTRY_PASS);
}
