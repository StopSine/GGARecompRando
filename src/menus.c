#include "gga_ap.h"
#include "recompui.h"
#include "menu_helpers.h"

void createUiFrame(RecompuiContext context, UiFrame* frame) {
    RecompuiColor bg_color     = { 255, 255, 255, (u8)(0.1f * 255) };
    RecompuiColor border_color = { 255, 255, 255, (u8)(0.7f * 255) };
    RecompuiColor modal_color  = { 8, 7, 13, (u8)(0.9f * 255) };

    const float modal_border_width = 5.1f;
    const float modal_border_radius = 16.0f;

    frame->root = recompui_context_root(context);
    recompui_set_position(frame->root, POSITION_ABSOLUTE);
    recompui_set_top(frame->root, 0, UNIT_DP);
    recompui_set_right(frame->root, 0, UNIT_DP);
    recompui_set_bottom(frame->root, 0, UNIT_DP);
    recompui_set_left(frame->root, 0, UNIT_DP);
    recompui_set_width_auto(frame->root);
    recompui_set_height_auto(frame->root);
    recompui_set_background_color(frame->root, &bg_color);
    recompui_set_display(frame->root, DISPLAY_FLEX);
    recompui_set_flex_direction(frame->root, FLEX_DIRECTION_COLUMN);
    recompui_set_justify_content(frame->root, JUSTIFY_CONTENT_CENTER);
    recompui_set_align_items(frame->root, ALIGN_ITEMS_CENTER);

    frame->container = recompui_create_element(context, frame->root);
    recompui_set_flex_grow(frame->container, 0.0f);
    recompui_set_flex_shrink(frame->container, 0.0f);
    recompui_set_width_auto(frame->container);
    recompui_set_height_auto(frame->container);
    recompui_set_display(frame->container, DISPLAY_BLOCK);
    recompui_set_padding(frame->container, 16.0f, UNIT_DP);
    recompui_set_align_items(frame->container, ALIGN_ITEMS_STRETCH);
    recompui_set_border_width(frame->container, modal_border_width, UNIT_DP);
    recompui_set_border_radius(frame->container, modal_border_radius, UNIT_DP);
    recompui_set_border_color(frame->container, &border_color);
    recompui_set_background_color(frame->container, &modal_color);
}
