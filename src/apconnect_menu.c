#include "gga_ap.h"
#include "apconnect_menu.h"
#include "recompui.h"
#include "recomputils.h"

static ApconnectMenu connect_menu;

bool apConnectPending = false;

static void connectPressed(RecompuiResource resource, const RecompuiEventData* data, void* userdata) {
    if (data->type != UI_EVENT_CLICK) {
        return;
    }

    char* server_text = recompui_get_input_text(connect_menu.server_textinput);
    char* slot_text = recompui_get_input_text(connect_menu.slot_textinput);
    char* password_text = recompui_get_input_text(connect_menu.password_textinput);

    rando_init(AP_GAME_NAME, server_text, slot_text, password_text);
    apConnectPending = true;

    recompui_close_context(connect_menu.context);
    randoEmitNormalNotification("Connecting...");
    recompui_open_context(connect_menu.context);

    recomp_free(server_text);
    recomp_free(slot_text);
    recomp_free(password_text);
}

static void testSendPressed(RecompuiResource resource, const RecompuiEventData* data, void* userdata) {
    if (data->type != UI_EVENT_CLICK) {
        return;
    }

    recompui_close_context(connect_menu.context);
    randoPollChecks();
    recompui_open_context(connect_menu.context);
}

static void closePressed(RecompuiResource resource, const RecompuiEventData* data, void* userdata) {
    if (data->type == UI_EVENT_CLICK) {
        recompui_hide_context(connect_menu.context);
    }
}

void randoCreateAPConnectMenu(void) {
    connect_menu.context = recompui_create_context();
    recompui_open_context(connect_menu.context);

    createUiFrame(connect_menu.context, &connect_menu.frame);

    connect_menu.title_label = recompui_create_label(connect_menu.context, connect_menu.frame.container,
                                                     "Archipelago", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.title_label, 40.0f, UNIT_DP);
    recompui_set_margin_bottom(connect_menu.title_label, 20.0f, UNIT_DP);

    connect_menu.server_label = recompui_create_label(connect_menu.context, connect_menu.frame.container,
                                                      "Server Address:Port", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.server_label, 30.0f, UNIT_DP);

    connect_menu.server_textinput = recompui_create_textinput(connect_menu.context, connect_menu.frame.container);
    recompui_set_font_size(connect_menu.server_textinput, 30.0f, UNIT_DP);
    recompui_set_input_text(connect_menu.server_textinput, "archipelago.gg:38281");
    recompui_set_margin_top(connect_menu.server_textinput, 10.0f, UNIT_DP);
    recompui_set_margin_bottom(connect_menu.server_textinput, 20.0f, UNIT_DP);
    recompui_set_max_width(connect_menu.server_textinput, 100.0f, UNIT_PERCENT);

    connect_menu.slotname_label = recompui_create_label(connect_menu.context, connect_menu.frame.container,
                                                        "Slot Name", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.slotname_label, 30.0f, UNIT_DP);

    connect_menu.slot_textinput = recompui_create_textinput(connect_menu.context, connect_menu.frame.container);
    recompui_set_font_size(connect_menu.slot_textinput, 30.0f, UNIT_DP);
    recompui_set_margin_top(connect_menu.slot_textinput, 10.0f, UNIT_DP);
    recompui_set_margin_bottom(connect_menu.slot_textinput, 20.0f, UNIT_DP);
    recompui_set_max_width(connect_menu.slot_textinput, 100.0f, UNIT_PERCENT);

    connect_menu.password_label = recompui_create_label(connect_menu.context, connect_menu.frame.container,
                                                        "Password", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.password_label, 30.0f, UNIT_DP);

    connect_menu.password_textinput = recompui_create_passwordinput(connect_menu.context, connect_menu.frame.container);
    recompui_set_font_size(connect_menu.password_textinput, 30.0f, UNIT_DP);
    recompui_set_margin_top(connect_menu.password_textinput, 10.0f, UNIT_DP);
    recompui_set_max_width(connect_menu.password_textinput, 100.0f, UNIT_PERCENT);

    connect_menu.connect_button = recompui_create_button(connect_menu.context, connect_menu.frame.container,
                                                         "Connect", BUTTONSTYLE_SECONDARY);
    recompui_set_text_align(connect_menu.connect_button, TEXT_ALIGN_CENTER);
    recompui_set_margin_top(connect_menu.connect_button, 70.0f, UNIT_DP);
    recompui_register_callback(connect_menu.connect_button, connectPressed, NULL);

    connect_menu.test_send_button = recompui_create_button(connect_menu.context, connect_menu.frame.container,
                                                           "Send Test Check", BUTTONSTYLE_SECONDARY);
    recompui_set_text_align(connect_menu.test_send_button, TEXT_ALIGN_CENTER);
    recompui_set_margin_top(connect_menu.test_send_button, 20.0f, UNIT_DP);
    recompui_register_callback(connect_menu.test_send_button, testSendPressed, NULL);

    connect_menu.close_button = recompui_create_button(connect_menu.context, connect_menu.frame.root,
                                                       "Close", BUTTONSTYLE_SECONDARY);
    recompui_set_position(connect_menu.close_button, POSITION_ABSOLUTE);
    recompui_set_left(connect_menu.close_button, 64.0f, UNIT_DP);
    recompui_set_top(connect_menu.close_button, 32.0f, UNIT_DP);
    recompui_register_callback(connect_menu.close_button, closePressed, NULL);

    recompui_close_context(connect_menu.context);
}

void randoShowAPConnectMenu(void) {
    recompui_show_context(connect_menu.context);
}
