
#include <MonoGUI.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void MonoGUI_init() {
	MonoGUI_idCounter = 0;
	MonoGUI_num_fonts = 0;
	MonoGUI_num_screens = 0;
	MonoGUI_curr_screen = -1;
	
	MonoGUI_selected_component = NULL;
}

void MonoGUI_render() {
	if (MonoGUI_curr_screen == -1) {
		return;
	}
	MonoGFX_clear();
	for (uint32_t i = 0; i < MonoGUI_screens[MonoGUI_curr_screen]->numComponents; i ++) {
		MonoGUI_component_render(MonoGUI_screens[MonoGUI_curr_screen]->components[i]);
	}
}
void MonoGUI_close_all_menus() {
	for (uint32_t i = 0; i < MonoGUI_num_screens; i ++) {
		for (uint32_t j = 0; j < MonoGUI_screens[i]->numComponents; j ++) {
			if (MonoGUI_screens[i]->components[j]->type == MonoGUI_COMPONENT_MENU_BUTTON) {
				MonoGUI_MenuButton* mb = (MonoGUI_MenuButton*)MonoGUI_screens[i]->components[j]->component;
				
				// if current menu contains selected item, deselect the item
				MonoGUI_MenuColumn* tmp = mb->menu->columns;
				if (tmp != NULL) {
					do {
						for (uint32_t k = 0; k < tmp->numRows; k ++) {
							if (tmp->rows[k]->selected) {
								MonoGUI_select_component(NULL);
							}
						}
						tmp = tmp->nextCol;
					} while(tmp != mb->menu->columns);
				}
				
				// close the menu
				if (mb->toggleButton->checked) {
					MonoGUI_toggleButton_click(mb->toggleButton);
				}
			}
		}
	}
}
void MonoGUI_set_screen(int32_t screen) {
	if ((uint32_t)screen > MonoGUI_num_screens) {
		return;
	}
	MonoGUI_close_all_menus();
	MonoGUI_select_component(NULL);
	MonoGUI_curr_screen = screen;
}

MonoGUI_Component* MonoGUI_component_set_select_state(MonoGUI_Component* component, uint8_t selectState) {
	if (component == NULL) {
		return NULL;
	}
	switch (component->type) {
		case MonoGUI_COMPONENT_BUTTON:
		case MonoGUI_COMPONENT_GRAPH_LABEL:
		case MonoGUI_COMPONENT_SCROLL_BUTTON:
		case MonoGUI_COMPONENT_RANGE:
		case MonoGUI_COMPONENT_TOGGLE_BUTTON:
		case MonoGUI_COMPONENT_RADIO_BUTTON:
		case MonoGUI_COMPONENT_MENU_BUTTON:
			component->selected = selectState;
			return component;
			break;
		case MonoGUI_COMPONENT_LABEL: // can't be selected
		case MonoGUI_COMPONENT_SPRITE:
		case MonoGUI_COMPONENT_GRAPH:
			break;
		case MonoGUI_COMPONENT_MENU_COLUMN: {
			if (((MonoGUI_MenuColumn*)component->component)->numRows) {
				return MonoGUI_component_set_select_state(((MonoGUI_MenuColumn*)component->component)->rows[0], selectState);
			}
		} break;
		case MonoGUI_COMPONENT_MENU: {
			if (((MonoGUI_Menu*)component->component)->columns != NULL) {
				if (((MonoGUI_Menu*)component->component)->columns->numRows) {
					return MonoGUI_component_set_select_state(((MonoGUI_Menu*)component->component)->columns->rows[0], selectState);
				}
			}
		} break;
		case MonoGUI_COMPONENT_SCREEN: {
			if (((MonoGUI_Screen*)component->component)->numComponents) {
				return MonoGUI_component_set_select_state(((MonoGUI_Screen*)component->component)->components[0], selectState);
			}
		} break;
	}
	return NULL;
}
void MonoGUI_select_component(MonoGUI_Component* component) {
	if (MonoGUI_selected_component != NULL) {
		MonoGUI_selected_component->selected = 0;
		MonoGUI_selected_component = NULL;
	}
	if (component != NULL) {
		MonoGUI_selected_component = MonoGUI_component_set_select_state(component, 1);
	}
}
void MonoGUI_click_component(MonoGUI_Component* component) {
	void* c = component->component;
	
	switch (component->type) {
		case MonoGUI_COMPONENT_BUTTON: {
			if (((MonoGUI_Button*)c)->onClick != NULL) {
				((MonoGUI_Button*)c)->onClick((MonoGUI_Button*)c);
			}
		} break;
		case MonoGUI_COMPONENT_SCROLL_BUTTON: {
			MonoGUI_close_all_menus();
			MonoGUI_select_component(component);
			if (((MonoGUI_ScrollButton*)c)->button->onClick != NULL) {
				((MonoGUI_ScrollButton*)c)->button->onClick((MonoGUI_ScrollButton*)c);
			}
		} break;
		case MonoGUI_COMPONENT_GRAPH_LABEL: {
			MonoGUI_close_all_menus();
			MonoGUI_select_component(component);
			if (((MonoGUI_GraphLabel*)c)->scrollButton->button->onClick) {
				((MonoGUI_GraphLabel*)c)->scrollButton->button->onClick((MonoGUI_GraphLabel*)c);
			}
		} break;
		case MonoGUI_COMPONENT_RANGE: {
			MonoGUI_close_all_menus();
			MonoGUI_select_component(component);
			if (((MonoGUI_Range*)c)->scrollButton->button->onClick) {
				((MonoGUI_Range*)c)->scrollButton->button->onClick((MonoGUI_Range*)c);
			}
		} break;
		case MonoGUI_COMPONENT_TOGGLE_BUTTON: { //
			// MonoGUI_close_all_menus(); // this would close menu when we are in it
			MonoGUI_toggleButton_click((MonoGUI_ToggleButton*)c);
		} break;
		case MonoGUI_COMPONENT_RADIO_BUTTON: {
			MonoGUI_radioButton_click((MonoGUI_RadioButton*)c);
		} break;
		case MonoGUI_COMPONENT_MENU_BUTTON:  {
			MonoGUI_menuButton_click((MonoGUI_MenuButton*)c); // MonoGUI_close_all_menus(); Handled inside, to enable toggle behavior 
		} break;
		default:
			break;
	}
}
void MonoGUI_tab(int16_t delta) {
	if (delta > 0) {
		MonoGUI_Component* tmp = MonoGUI_selected_component->tabNext;
		while (tmp != NULL && tmp->visible == 0) { // find next non null, visible component
			if (tmp == MonoGUI_selected_component) { // prevent infinite cycle, if we reach the first element, return
				return;
			}
			tmp = tmp->tabNext;
		}
		MonoGUI_select_component(tmp);
	} else if (delta < 0) {
		MonoGUI_Component* tmp = MonoGUI_selected_component->tabPrev;
		while (tmp != NULL && tmp->visible == 0) { // find prev non null, visible component
			if (tmp == MonoGUI_selected_component) { // prevent infinite cycle, if we reach the first element, return
				return;
			}
			tmp = tmp->tabPrev;
		}
		MonoGUI_select_component(MonoGUI_selected_component->tabPrev);
	}
}
void MonoGUI_scroll(int16_t delta, uint8_t largeStep) { // called externally
	if (MonoGUI_selected_component == NULL) {
		return;
	}
	switch (MonoGUI_selected_component->type) {
		case MonoGUI_COMPONENT_SCROLL_BUTTON:
			MonoGUI_scrollButton_scroll((MonoGUI_ScrollButton*)MonoGUI_selected_component->component, delta, largeStep);
			break;
		case MonoGUI_COMPONENT_RANGE:
			MonoGUI_scrollButton_scroll(((MonoGUI_Range*)MonoGUI_selected_component->component)->scrollButton, delta, largeStep);
			break;
		case MonoGUI_COMPONENT_GRAPH_LABEL:
			MonoGUI_scrollButton_scroll(((MonoGUI_GraphLabel*)MonoGUI_selected_component->component)->scrollButton, delta, largeStep);
			break;
		default:
			MonoGUI_tab(delta);
		break;
	}
}
void MonoGUI_mainBtnClick() { // called externally
	if (MonoGUI_selected_component == NULL) {
		return;
	}
	MonoGUI_click_component(MonoGUI_selected_component);
}


// string handling
uint32_t MonoGUI_register_font(MonoGUI_FontInfoTypedef* font) {
	MonoGUI_fonts[MonoGUI_num_fonts] = font;
	
	MonoGUI_num_fonts ++;
	return MonoGUI_num_fonts - 1;
}
MonoGUI_FontInfoTypedef * MonoGUI_get_font(uint32_t fontID) {
	if (fontID > MonoGUI_num_fonts) {
		return NULL;
	}
	return MonoGUI_fonts[fontID];
}
uint32_t MonoGUI_get_string_width(char* str, uint32_t fontID) {
	uint32_t map;
	uint32_t allwidth = 0;
	const MonoGUI_FontInfoTypedef* font = MonoGUI_get_font(fontID);
	
	while((map = *str++)) {
		map -= font->start_char;
		if (map > font->end_char) { // map < 0 || 
			continue; // character not in table
		}
		allwidth += MONO_GUI_GET_CHARACTER_WIDTH(font, map) + 1;
	}
	return allwidth;
}
uint32_t MonoGUI_get_string_height(char* str, uint32_t fontID) {
	return strlen(str) * MonoGUI_get_font(fontID)->glyph_height;
}
uint32_t MonoGUI_write_string(uint32_t x, uint32_t y, char* str, uint32_t fontID, MonoGUI_TextAlignTypedef align, MonoGUI_TextDirectionTypedef dir, uint8_t color) {
	if (dir == MonoGUI_TEXT_DIRECTION_HORIZONTAL) {
		switch (align) {
			case MonoGUI_TEXT_ALIGN_LEFT:
			case MonoGUI_TEXT_ALIGN_TOP:
			case MonoGUI_TEXT_ALIGN_BOTTOM:
				break;
			case MonoGUI_TEXT_ALIGN_CENTER:
				x -= MonoGUI_get_string_width(str, fontID) / 2;
				break;
			case MonoGUI_TEXT_ALIGN_RIGHT:
				x -= MonoGUI_get_string_width(str, fontID);
				break;
		}
	} else {
		switch (align) {
			case MonoGUI_TEXT_ALIGN_TOP:
			case MonoGUI_TEXT_ALIGN_LEFT:
			case MonoGUI_TEXT_ALIGN_RIGHT:
				break;
			case MonoGUI_TEXT_ALIGN_CENTER:
				y -= MonoGUI_get_string_height(str, fontID) / 2;
				break;
			case MonoGUI_TEXT_ALIGN_BOTTOM:
				y -= MonoGUI_get_string_height(str, fontID);
				break;
		}
	}
	
	MonoGUI_FontInfoTypedef * font = MonoGUI_get_font(fontID);
	uint32_t offset;
	uint32_t width, by = 0, mask = 0;
	uint32_t NrBytes;
	uint32_t i, j, height, allheight = 0, allwidth = 0;
	uint32_t map;
	
	while((map = *str++)) {
		map -= font->start_char;
		if (map > font->end_char) { // map < 0 || 
			continue; // character not in table
		}
		
		width = MONO_GUI_GET_CHARACTER_WIDTH(font, map);
		offset = MONO_GUI_GET_CHARACTER_OFFSET(font, map);
		height = font->glyph_height;

		NrBytes = ((width - 1) / 8) + 1;
		
		for (j = 0; j < height * NrBytes; j += NrBytes) {
			for (i = 0; i < width; i++) {
				if (i % 8 == 0) {
					by = MONO_GUI_READ_BYTE_FROM_PROGMEM(font->glyph_table[offset + j + (i / 8)]);
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
		if (dir == MonoGUI_TEXT_DIRECTION_HORIZONTAL) {
			allwidth += width + 1; // TODO: (MISSING) text spacing
		} else {
			allheight += height + 1; // TODO: (MISSING) text spacing
		}
	}
	return (dir == MonoGUI_TEXT_DIRECTION_HORIZONTAL) ? allwidth : allheight;
}










