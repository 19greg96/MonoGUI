#ifndef __GUI_H
#define __GUI_H

#include "GUI_font.h"
#include "GUI_components.h"
#include "BSP_GLCD.h"
#include <stdint.h>



uint32_t GUI_idCounter;
#define GUI_MAX_NUM_SCREENS		5
GUI_Screen* GUI_screens[GUI_MAX_NUM_SCREENS];
uint32_t GUI_num_screens;
int32_t GUI_curr_screen;
GUI_Component* GUI_selected_component;

#define GUI_MAX_FONTS	2
GUI_FontInfoTypedef * GUI_fonts[GUI_MAX_FONTS];
uint32_t GUI_num_fonts;

void GUI_init();
void GUI_render();
void GUI_close_all_menus();
void GUI_select_component(GUI_Component* component);
void GUI_click_component(GUI_Component* component);
GUI_Component* GUI_component_set_select_state(GUI_Component* component, uint8_t selectState);
void GUI_tab(int16_t delta);
void GUI_scroll(int16_t delta, uint8_t largeStep);
void GUI_mainBtnClick();
void GUI_set_screen(int32_t screen);

uint32_t GUI_register_font(GUI_FontInfoTypedef * font);
GUI_FontInfoTypedef * GUI_get_font(uint32_t fontID);
uint32_t GUI_get_string_width(char * str, uint32_t fontID);
uint32_t GUI_get_string_height(char * str, uint32_t fontID);
uint32_t GUI_write_string(uint32_t x, uint32_t y, char * str, uint32_t fontID, GUI_TextAlignTypedef align, GUI_TextDirectionTypedef dir, uint8_t color);

#endif
