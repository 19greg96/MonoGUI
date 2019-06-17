
#include "GUI.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void GUI_init() {
	GUI_idCounter = 0;
	GUI_num_screens = 0;
	GUI_curr_screen = -1;
	
	GUI_selected_component = NULL;
}

void GUI_render() {
	if (GUI_curr_screen == -1) {
		return;
	}
	MonoGFX_clear();
	for (uint32_t i = 0; i < GUI_screens[GUI_curr_screen]->numComponents; i ++) {
		GUI_component_render(GUI_screens[GUI_curr_screen]->components[i]);
	}
}
void GUI_close_all_menus() {
	for (uint32_t i = 0; i < GUI_num_screens; i ++) {
		for (uint32_t j = 0; j < GUI_screens[i]->numComponents; j ++) {
			if (GUI_screens[i]->components[j]->type == GUI_COMPONENT_MENU_BUTTON) {
				GUI_MenuButton* mb = (GUI_MenuButton*)GUI_screens[i]->components[j]->component;
				
				// if current menu contains selected item, deselect the item
				GUI_MenuColumn* tmp = mb->menu->columns;
				if (tmp != NULL) {
					do {
						for (uint32_t k = 0; k < tmp->numRows; k ++) {
							if (tmp->rows[k]->selected) {
								GUI_select_component(NULL);
							}
						}
						tmp = tmp->nextCol;
					} while(tmp != mb->menu->columns);
				}
				
				// close the menu
				if (mb->toggleButton->checked) {
					GUI_toggleButton_click(mb->toggleButton);
				}
			}
		}
	}
}
void GUI_set_screen(int32_t screen) {
	if (screen > GUI_num_screens) {
		return;
	}
	GUI_close_all_menus();
	GUI_select_component(NULL);
	GUI_curr_screen = screen;
}

GUI_Component* GUI_component_set_select_state(GUI_Component* component, uint8_t selectState) {
	if (component == NULL) {
		return NULL;
	}
	switch (component->type) {
		case GUI_COMPONENT_BUTTON:
		case GUI_COMPONENT_GRAPH_LABEL:
		case GUI_COMPONENT_SCROLL_BUTTON:
		case GUI_COMPONENT_RANGE:
		case GUI_COMPONENT_TOGGLE_BUTTON:
		case GUI_COMPONENT_RADIO_BUTTON:
		case GUI_COMPONENT_MENU_BUTTON:
			component->selected = selectState;
			return component;
			break;
		case GUI_COMPONENT_LABEL: // can't be selected
		case GUI_COMPONENT_SPRITE:
		case GUI_COMPONENT_GRAPH:
			break;
		case GUI_COMPONENT_MENU_COLUMN: {
			if (((GUI_MenuColumn*)component->component)->numRows) {
				return GUI_component_set_select_state(((GUI_MenuColumn*)component->component)->rows[0], selectState);
			}
		} break;
		case GUI_COMPONENT_MENU: {
			if (((GUI_Menu*)component->component)->columns != NULL) {
				if (((GUI_Menu*)component->component)->columns->numRows) {
					return GUI_component_set_select_state(((GUI_Menu*)component->component)->columns->rows[0], selectState);
				}
			}
		} break;
		case GUI_COMPONENT_SCREEN: {
			if (((GUI_Screen*)component->component)->numComponents) {
				return GUI_component_set_select_state(((GUI_Screen*)component->component)->components[0], selectState);
			}
		} break;
	}
	return NULL;
}
void GUI_select_component(GUI_Component* component) {
	if (GUI_selected_component != NULL) {
		GUI_selected_component->selected = 0;
		GUI_selected_component = NULL;
	}
	if (component != NULL) {
		GUI_selected_component = GUI_component_set_select_state(component, 1);
	}
}
void GUI_click_component(GUI_Component* component) {
	void* c = component->component;
	
	switch (component->type) {
		case GUI_COMPONENT_BUTTON: {
			if (((GUI_Button*)c)->onClick != NULL) {
				((GUI_Button*)c)->onClick((GUI_Button*)c);
			}
		} break;
		case GUI_COMPONENT_SCROLL_BUTTON: {
			GUI_close_all_menus();
			GUI_select_component(component);
			if (((GUI_ScrollButton*)c)->button->onClick != NULL) {
				((GUI_ScrollButton*)c)->button->onClick((GUI_ScrollButton*)c);
			}
		} break;
		case GUI_COMPONENT_GRAPH_LABEL: {
			GUI_close_all_menus();
			GUI_select_component(component);
			if (((GUI_GraphLabel*)c)->scrollButton->button->onClick) {
				((GUI_GraphLabel*)c)->scrollButton->button->onClick((GUI_GraphLabel*)c);
			}
		} break;
		case GUI_COMPONENT_RANGE: {
			GUI_close_all_menus();
			GUI_select_component(component);
			if (((GUI_Range*)c)->scrollButton->button->onClick) {
				((GUI_Range*)c)->scrollButton->button->onClick((GUI_Range*)c);
			}
		} break;
		case GUI_COMPONENT_TOGGLE_BUTTON: { //
			// GUI_close_all_menus(); // this would close menu when we are in it
			GUI_toggleButton_click((GUI_ToggleButton*)c);
		} break;
		case GUI_COMPONENT_RADIO_BUTTON: {
			GUI_radioButton_click((GUI_RadioButton*)c);
		} break;
		case GUI_COMPONENT_MENU_BUTTON:  {
			GUI_menuButton_click((GUI_MenuButton*)c); // GUI_close_all_menus(); Handled inside, to enable toggle behavior 
		} break;
		default:
			break;
	}
}
void GUI_tab(int16_t delta) {
	if (delta > 0) {
		GUI_Component* tmp = GUI_selected_component->tabNext;
		while (tmp != NULL && tmp->visible == 0) { // find next non null, visible component
			if (tmp == GUI_selected_component) { // prevent infinite cycle, if we reach the first element, return
				return;
			}
			tmp = tmp->tabNext;
		}
		GUI_select_component(tmp);
	} else if (delta < 0) {
		GUI_Component* tmp = GUI_selected_component->tabPrev;
		while (tmp != NULL && tmp->visible == 0) { // find prev non null, visible component
			if (tmp == GUI_selected_component) { // prevent infinite cycle, if we reach the first element, return
				return;
			}
			tmp = tmp->tabPrev;
		}
		GUI_select_component(GUI_selected_component->tabPrev);
	}
}
void GUI_scroll(int16_t delta, uint8_t largeStep) { // called externally
	if (GUI_selected_component == NULL) {
		return;
	}
	switch (GUI_selected_component->type) {
		case GUI_COMPONENT_SCROLL_BUTTON:
			GUI_scrollButton_scroll((GUI_ScrollButton*)GUI_selected_component->component, delta, largeStep);
			break;
		case GUI_COMPONENT_RANGE:
			GUI_scrollButton_scroll(((GUI_Range*)GUI_selected_component->component)->scrollButton, delta, largeStep);
			break;
		case GUI_COMPONENT_GRAPH_LABEL:
			GUI_scrollButton_scroll(((GUI_GraphLabel*)GUI_selected_component->component)->scrollButton, delta, largeStep);
			break;
		default:
			GUI_tab(delta);
		break;
	}
}
void GUI_mainBtnClick() { // called externally
	if (GUI_selected_component == NULL) {
		return;
	}
	GUI_click_component(GUI_selected_component);
}


uint32_t GUI_register_font(GUI_FontInfoTypedef * font) {
	GUI_fonts[GUI_num_fonts] = font;
	
	GUI_num_fonts ++;
	return GUI_num_fonts - 1;
}
GUI_FontInfoTypedef * GUI_get_font(uint32_t fontID) {
	if (fontID < 0) {
		fontID = 0;
	} else if (fontID >= GUI_num_fonts) {
		fontID = GUI_num_fonts - 1;
	}
	return GUI_fonts[fontID];
}
uint32_t GUI_get_string_width(char * str, uint32_t fontID) {
	unsigned int map, allwidth = 0;
	const GUI_FontInfoTypedef * font = GUI_get_font(fontID);
	
	while((map = *str++)) {
		map -= font->start_char;
		if (map < 0 || map > font->end_char) {
			continue;
		}
		allwidth += font->mapping_table[map].widthBits + 1;
	}
	return allwidth;
}
uint32_t GUI_get_string_height(char * str, uint32_t fontID) {
	return strlen(str) * GUI_get_font(fontID)->glyph_height;
}
uint32_t GUI_write_string(uint32_t x, uint32_t y, char * str, uint32_t fontID, GUI_TextAlignTypedef align, GUI_TextDirectionTypedef dir, uint8_t color) {
	if (dir == GUI_TEXT_DIRECTION_HORIZONTAL) {
		switch (align) {
			case GUI_TEXT_ALIGN_LEFT:
			case GUI_TEXT_ALIGN_TOP:
			case GUI_TEXT_ALIGN_BOTTOM:
				break;
			case GUI_TEXT_ALIGN_CENTER:
				x -= GUI_get_string_width(str, fontID) / 2;
				break;
			case GUI_TEXT_ALIGN_RIGHT:
				x -= GUI_get_string_width(str, fontID);
				break;
		}
	} else {
		switch (align) {
			case GUI_TEXT_ALIGN_TOP:
			case GUI_TEXT_ALIGN_LEFT:
			case GUI_TEXT_ALIGN_RIGHT:
				break;
			case GUI_TEXT_ALIGN_CENTER:
				y -= GUI_get_string_height(str, fontID) / 2;
				break;
			case GUI_TEXT_ALIGN_BOTTOM:
				y -= GUI_get_string_height(str, fontID);
				break;
		}
	}
	
	GUI_FontInfoTypedef * font = GUI_get_font(fontID);
	uint32_t offset;
	uint32_t width, by = 0, mask = 0;
	uint32_t NrBytes;
	uint32_t i, j, height, allheight = 0, allwidth = 0;
	int8_t map;
	
	while((map = *str++)) {
		map -= font->start_char;
		if (map < 0 || map > font->end_char) {
			continue;
		}
		
		width = font->mapping_table[map].widthBits;
		offset = font->mapping_table[map].offset;
		height = font->glyph_height;

		NrBytes = ((width - 1) / 8) + 1;
		
		for (j = 0; j < height * NrBytes; j += NrBytes) {
			for (i = 0; i < width; i++) {
				if (i % 8 == 0) {
					by = font->glyph_table[offset + j + (i / 8)];
					mask = 0x80;
				}
				
				if (by & mask) {
					MonoGFX_set_pixel(x + i + allwidth, y + j / NrBytes + allheight, color);
				} else if (color != 2) {
					MonoGFX_set_pixel(x + i + allwidth, y + j / NrBytes + allheight, !color);
				}
				
	 			mask >>= 1;
			}
		}
		if (dir == GUI_TEXT_DIRECTION_HORIZONTAL) {
			allwidth += width + 1; // TODO: (MISSING) text spacing
		} else {
			allheight += height + 1; // TODO: (MISSING) text spacing
		}
	}
	return (dir == GUI_TEXT_DIRECTION_HORIZONTAL) ? allwidth : allheight;
}










