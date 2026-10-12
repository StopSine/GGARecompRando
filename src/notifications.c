#include "gga_ap.h"
#include "recompui.h"

#define MAX_NOTIFICATIONS 8
#define NOTIFICATION_TRUE_DURATION(seconds) (20 * (seconds))
#define NOTIFICATION_DURATION NOTIFICATION_TRUE_DURATION(10)
#define NOTIFICATION_FADE_START NOTIFICATION_TRUE_DURATION(2)

#define NOTIFICATION_WIDTH 400.0f
#define NOTIFICATION_PADDING 12.0f
#define LABEL_WORD_GAP 6.0f

static RecompuiContext notif_context;
static RecompuiResource notif_root;
static RecompuiResource notif_container;

static RecompuiColor notifBg = { 0, 0, 0, (u8)(0.8f * 255) };
static RecompuiColor notifBgError = { 128, 0, 0, (u8)(0.8f * 255) };
static RecompuiColor msgTextColor = { 255, 255, 255, 255 };
static RecompuiColor playerTextColor = { 0, 255, 127, 255 };

static RecompuiColor progTextColor = { 175, 153, 239, 255 };
static RecompuiColor usefulTextColor = { 109, 139, 232, 255 };
static RecompuiColor junkTextColor = { 0, 238, 238, 255 };
static RecompuiColor trapTextColor = { 255, 119, 0, 255 };

typedef struct {
    RecompuiResource notification;
    int duration;
} Notification;

static int notif_head = 0;
static int notif_count = 0;
static Notification notifications[MAX_NOTIFICATIONS];

static RecompuiColor* classification_color(RandoItemClassification item_class) {
    switch (item_class) {
        case RANDO_ITEM_CLASS_PROGRESSION: return &progTextColor;
        case RANDO_ITEM_CLASS_USEFUL:      return &usefulTextColor;
        case RANDO_ITEM_CLASS_TRAP:        return &trapTextColor;
        default:                           return &junkTextColor;
    }
}

void notificationUpdateCycle(void) {
    int start_index = notif_head;

    for (int i = 0; i < notif_count; i++) {
        int index = (start_index + i) % MAX_NOTIFICATIONS;

        if (notifications[index].duration <= 0) {
            continue;
        }
        notifications[index].duration--;

        if (notifications[index].duration <= 0) {
            recompui_open_context(notif_context);
            recompui_destroy_element(notif_container, notifications[index].notification);
            recompui_close_context(notif_context);

            if (index == notif_head) {
                notif_head = (notif_head + 1) % MAX_NOTIFICATIONS;
                notif_count--;
            }
        }
        else if (notifications[index].duration < NOTIFICATION_FADE_START) {
            recompui_open_context(notif_context);
            recompui_set_opacity(notifications[index].notification,
                                 (float)notifications[index].duration / NOTIFICATION_FADE_START);
            recompui_close_context(notif_context);
        }
    }
}

void randoCreateNotificationContainer(void) {
    const float body_padding = 32.0f;

    notif_context = recompui_create_context();
    recompui_set_context_captures_input(notif_context, 0);
    recompui_set_context_captures_mouse(notif_context, 0);

    recompui_open_context(notif_context);

    notif_root = recompui_context_root(notif_context);
    recompui_set_position(notif_root, POSITION_ABSOLUTE);
    recompui_set_top(notif_root, 0, UNIT_DP);
    recompui_set_right(notif_root, 0, UNIT_DP);
    recompui_set_bottom(notif_root, 0, UNIT_DP);
    recompui_set_left(notif_root, 0, UNIT_DP);
    recompui_set_width_auto(notif_root);
    recompui_set_height_auto(notif_root);
    recompui_set_padding(notif_root, body_padding, UNIT_DP);
    recompui_set_flex_direction(notif_root, FLEX_DIRECTION_COLUMN);
    recompui_set_justify_content(notif_root, JUSTIFY_CONTENT_CENTER);

    notif_container = recompui_create_element(notif_context, notif_root);
    recompui_set_height(notif_container, 100.0f, UNIT_PERCENT);
    recompui_set_flex_grow(notif_container, 1.0f);
    recompui_set_display(notif_container, DISPLAY_FLEX);
    recompui_set_gap(notif_container, 16.0f, UNIT_DP);
    recompui_set_justify_content(notif_container, JUSTIFY_CONTENT_FLEX_START);

    recompui_set_flex_direction(notif_container, FLEX_DIRECTION_COLUMN);
    recompui_set_align_items(notif_container, ALIGN_ITEMS_FLEX_END);

    recompui_close_context(notif_context);
    recompui_show_context(notif_context);
}

static RecompuiResource create_basic_notification_element(void) {
    if (notif_count == MAX_NOTIFICATIONS) {
        recompui_destroy_element(notif_container, notifications[notif_head].notification);
        notif_head = (notif_head + 1) % MAX_NOTIFICATIONS;
        notif_count--;
    }

    int index = (notif_head + notif_count) % MAX_NOTIFICATIONS;

    notifications[index].notification = recompui_create_element(notif_context, notif_container);
    notifications[index].duration = NOTIFICATION_DURATION;

    RecompuiResource notification = notifications[index].notification;
    recompui_set_padding(notification, NOTIFICATION_PADDING, UNIT_DP);
    recompui_set_border_radius(notification, 12.0f, UNIT_DP);
    recompui_set_background_color(notification, &notifBg);
    recompui_set_display(notification, DISPLAY_FLEX);
    recompui_set_max_width(notification, NOTIFICATION_WIDTH, UNIT_DP);
    recompui_set_width(notification, NOTIFICATION_WIDTH, UNIT_DP);
    recompui_set_justify_content(notification, JUSTIFY_CONTENT_CENTER);
    recompui_set_align_items(notification, ALIGN_ITEMS_CENTER);

    notif_count++;

    return notification;
}

void randoEmitReceiveNotification(const char* item_name, const char* from_name, RandoItemClassification item_class) {
    recompui_open_context(notif_context);

    RecompuiResource notification = create_basic_notification_element();

    RecompuiResource text_got = recompui_create_label(notif_context, notification, "Got ", LABELSTYLE_SMALL);
    recompui_set_color(text_got, &msgTextColor);

    RecompuiResource text_item = recompui_create_label(notif_context, notification, item_name, LABELSTYLE_SMALL);
    recompui_set_color(text_item, classification_color(item_class));

    // Each label is its own flex item, so a leading space in the text collapses.
    RecompuiResource text_from = recompui_create_label(notif_context, notification, " from ", LABELSTYLE_SMALL);
    recompui_set_color(text_from, &msgTextColor);
    recompui_set_margin_left(text_from, LABEL_WORD_GAP, UNIT_DP);

    RecompuiResource text_player = recompui_create_label(notif_context, notification, from_name, LABELSTYLE_SMALL);
    recompui_set_color(text_player, &playerTextColor);

    recompui_close_context(notif_context);
}

void randoEmitSendNotification(const char* item_name, const char* to_name, RandoItemClassification item_class) {
    recompui_open_context(notif_context);

    RecompuiResource notification = create_basic_notification_element();

    RecompuiResource text_sent = recompui_create_label(notif_context, notification, "Sent ", LABELSTYLE_SMALL);
    recompui_set_color(text_sent, &msgTextColor);

    RecompuiResource text_item = recompui_create_label(notif_context, notification, item_name, LABELSTYLE_SMALL);
    recompui_set_color(text_item, classification_color(item_class));

    RecompuiResource text_to = recompui_create_label(notif_context, notification, " to ", LABELSTYLE_SMALL);
    recompui_set_color(text_to, &msgTextColor);
    recompui_set_margin_left(text_to, LABEL_WORD_GAP, UNIT_DP);

    RecompuiResource text_player = recompui_create_label(notif_context, notification, to_name, LABELSTYLE_SMALL);
    recompui_set_color(text_player, &playerTextColor);

    recompui_close_context(notif_context);
}

void randoEmitNormalNotification(const char* notif_text) {
    recompui_open_context(notif_context);

    RecompuiResource notification = create_basic_notification_element();
    RecompuiResource text = recompui_create_label(notif_context, notification, notif_text, LABELSTYLE_SMALL);
    recompui_set_color(text, &msgTextColor);

    recompui_close_context(notif_context);
}

void randoEmitErrorNotification(const char* error_text) {
    recompui_open_context(notif_context);

    RecompuiResource notification = create_basic_notification_element();
    recompui_set_background_color(notification, &notifBgError);

    RecompuiResource text = recompui_create_label(notif_context, notification, error_text, LABELSTYLE_SMALL);
    recompui_set_color(text, &msgTextColor);

    recompui_close_context(notif_context);
}
