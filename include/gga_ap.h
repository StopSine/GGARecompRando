#ifndef __GGA_AP_H__
#define __GGA_AP_H__

#include "modding.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef float f32;

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#define NULL ((void*)0)
#endif

#define AP_GAME_NAME "Goemon's Great Adventure Recompiled"

// A location id is the position of its flag bit in the save, so neither side
// needs a table: location_id = (flag_byte - AP_FLAG_BLOCK) * 8 + bit.
// See lib/gga/docs/save_memory_map.md in the Goemon64Recomp repo.
#define AP_FLAG_BLOCK 0x8008821C
#define AP_FLAG_BLOCK_SIZE 0x14

// The only check outside the block. Yae needs no equivalent: joining also sets
// a bit inside it.
#define AP_SASUKE_BYTE 0x80088317
#define AP_SASUKE_BIT 7

// 0 on entry, 1 once the first boss is down, 2 when the castle is finished.
// Dream Castle has no cleared flag, so its counter is what says the game is won.
#define AP_DREAM_CASTLE_PROGRESS 0x80088315
#define AP_CASTLE_PROGRESS_DONE 2

// Player 1 health, 0 to 3.
#define AP_P1_HEALTH 0x80088284

// One byte per world. The count is the reward; the flag bit only records where
// it came from.
#define AP_PASS_COUNT_BASE 0x80088276
#define AP_PASS_COUNT_WORLDS 6

#define AP_ITEM_ENTRY_PASS 1
#define AP_ITEM_RYO 2
#define AP_ITEM_CHAIN_PIPE 3
#define AP_ITEM_YAE 4
#define AP_ITEM_RYUGU_ACCESS 5
#define AP_ITEM_MAFU_ACCESS 6
#define AP_ITEM_UNDERWORLD_ACCESS 7
#define AP_ITEM_FLOATING_ACCESS 8

RECOMP_IMPORT(".", void rando_init(const char* game_name, const char* address, const char* player_name, const char* password));
RECOMP_IMPORT(".", bool rando_is_connected());
RECOMP_IMPORT(".", bool rando_connect_failed());
RECOMP_IMPORT(".", u32 rando_get_items_size());
RECOMP_IMPORT(".", u32 rando_get_item(u32 items_i));
RECOMP_IMPORT(".", u32 rando_get_sending_player(u32 items_i));
RECOMP_IMPORT(".", void rando_get_item_name_from_id(u32 item_id, char* out_str));
RECOMP_IMPORT(".", void rando_get_sending_player_name(u32 items_i, char* out_str));
RECOMP_IMPORT(".", void rando_send_location(u32 location_id));
// These read the scout reply, so they are empty until the scouts come back.
RECOMP_IMPORT(".", void rando_get_location_item_name(u32 location_id, char* out_str));
RECOMP_IMPORT(".", void rando_get_location_item_player(u32 location_id, char* out_str));
RECOMP_IMPORT(".", u32 rando_get_location_type(u32 location_id));
RECOMP_IMPORT(".", void rando_queue_scouts_all());
RECOMP_IMPORT(".", void rando_send_queued_scouts(u32 hint));
RECOMP_IMPORT(".", bool rando_is_scouted());
RECOMP_IMPORT(".", bool rando_location_is_checked(u32 location_id));
RECOMP_IMPORT(".", u32 rando_has_item(u32 item_id));
RECOMP_IMPORT(".", void rando_get_own_slot_name(char* out_str));
RECOMP_IMPORT(".", u32 rando_get_seed_name(char* seed_name_out, u32 buffer_size));
RECOMP_IMPORT(".", void rando_complete_goal());
RECOMP_IMPORT(".", bool rando_get_death_link_enabled());
RECOMP_IMPORT(".", bool rando_get_death_link_pending());
RECOMP_IMPORT(".", void rando_reset_death_link_pending());
RECOMP_IMPORT(".", void rando_send_death_link());

// The host queues UI events and only dispatches them when guest code drains
// them. Nothing in Goemon64Recomp does, so the mod has to.
RECOMP_IMPORT("*", void recomp_run_ui_callbacks());

typedef enum {
    RANDO_ITEM_CLASS_PROGRESSION,
    RANDO_ITEM_CLASS_USEFUL,
    RANDO_ITEM_CLASS_JUNK,
    RANDO_ITEM_CLASS_TRAP
} RandoItemClassification;

extern bool apConnected;

void randoCreateNotificationContainer(void);
void notificationUpdateCycle(void);
void randoEmitReceiveNotification(const char* item_name, const char* from_name, RandoItemClassification item_class);
void randoEmitSendNotification(const char* item_name, const char* to_name, RandoItemClassification item_class);
void randoEmitNormalNotification(const char* notif_text);
void randoEmitErrorNotification(const char* error_text);

void randoCreateAPConnectMenu(void);
void randoShowAPConnectMenu(void);
void randoHideAPConnectMenu(void);

void randoItemGive(u32 ap_item_id);
void randoApplyWorldAccess(void);
void randoPollChecks(void);
void randoPollDeathLink(void);

#endif
