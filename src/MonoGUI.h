#ifndef __MonoGUI_H
#define __MonoGUI_H

#include "MonoGUI_config.h"
#include "MonoGUI_font.h"
#include "MonoGUI_components.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t MonoGUI_idCounter;
#define MonoGUI_MAX_NUM_SCREENS		5
MonoGUI_Screen* MonoGUI_screens[MonoGUI_MAX_NUM_SCREENS];
uint32_t MonoGUI_num_screens;
int32_t MonoGUI_curr_screen;
MonoGUI_Component* MonoGUI_selected_component;

#define MonoGUI_MAX_FONTS	2
MonoGUI_FontInfoTypedef* MonoGUI_fonts[MonoGUI_MAX_FONTS];
uint32_t MonoGUI_num_fonts;

void MonoGUI_init();
void MonoGUI_render();
void MonoGUI_close_all_menus();
void MonoGUI_select_component(MonoGUI_Component* component);
void MonoGUI_click_component(MonoGUI_Component* component);
MonoGUI_Component* MonoGUI_component_set_select_state(MonoGUI_Component* component, uint8_t selectState);
void MonoGUI_tab(int16_t delta);
void MonoGUI_scroll(int16_t delta, uint8_t largeStep);
void MonoGUI_mainBtnClick();
void MonoGUI_set_screen(int32_t screen);

#define MonoGUI_TEXT_SPACING	1

uint32_t MonoGUI_register_font(MonoGUI_FontInfoTypedef * font);
MonoGUI_FontInfoTypedef * MonoGUI_get_font(uint32_t fontID);
uint32_t MonoGUI_get_string_width(char * str, uint32_t fontID);
uint32_t MonoGUI_get_string_height(char * str, uint32_t fontID);
uint32_t MonoGUI_write_string(uint32_t x, uint32_t y, char * str, uint32_t fontID, MonoGUI_TextAlignTypedef align, MonoGUI_TextDirectionTypedef dir, uint8_t color);

#ifdef __cplusplus
}
#endif

#endif
