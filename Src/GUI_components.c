
#include "GUI.h"
#include "GUI_components.h"
#include "BSP_ADC.h"
#include "GUI_font_legible3x5_6pt.h"
#include <stdlib.h>
#include <string.h>

#define GUI_ROW_WIDTH		32
#define GUI_ROW_HEIGHT		8
#define GUI_LEFT_PADDING	0

void formatPercentage(char* out, void* param) {
	sprintf(out, "%0.1f%%", *((float*)param));
}
void formatVoltage(char* out, void* param) {
	float voltage = *((float*)param);
	if (voltage < 0.1f) {
		sprintf(out, "%dmV", (int)(voltage * 1000.0f));
		return;
	} else if (voltage < 10.0f) {
		sprintf(out, "%.2fV", voltage);
		return;
	}
	sprintf(out, "%dV", (int)voltage);
}
void formatFrequency(char* out, void* param) {
	float frequency_Hz = *((float*)param);
	if (frequency_Hz < 10.0f) {
		sprintf(out, "%0.1fHz", frequency_Hz);
	} else if (frequency_Hz < 100.0f) {
		sprintf(out, "%0.1fHz", frequency_Hz);
	} else if (frequency_Hz < 1000.0f) {
		sprintf(out, "%0.1fHz", frequency_Hz);
	} else if (frequency_Hz < 10000.0f) {
		sprintf(out, "%0.1fkHz", frequency_Hz / 1000.0f); // kHz
	} else if (frequency_Hz < 100000.0f) {
		sprintf(out, "%0.1fkHz", frequency_Hz / 1000.0f);
	} else if (frequency_Hz < 1000000.0f) {
		sprintf(out, "%0.1fkHz", frequency_Hz / 1000.0f);
	} else {
		sprintf(out, "%0.1fMHz", frequency_Hz / 1000000.0f); // MHz
	}
}
void formatTime(char* out, void* param) {
	float time_s = *((float*)param);
	if (time_s < 0.000001f) { // ns
		sprintf(out, "%0.0fns", time_s * 1000000000.0f);
	} else if (time_s < 0.001f) { // us
		sprintf(out, "%0.0fus", time_s * 1000000.0f);
	} else if (time_s < 0.1f) { // ms
		sprintf(out, "%0.0fms", time_s * 1000.0f);
	} else {
		sprintf(out, "%0.2fs", time_s);
	}
}
void formatSimpleText(char* out, void* param) {
	sprintf(out, "%s", (char*)param);
}
void formatEmpty(char* out, void* param) {
	strcpy(out, "");
}

void GUI_drawPanelTriangle(int32_t x, int32_t y, uint8_t triangleFill) {
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 2, y - 5, triangleFill);
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 3, y - 5, triangleFill);
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 4, y - 5, triangleFill); // bottom border of menu
	
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 3, y - 4, triangleFill); // center of triangle is white
	
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 2, y - 4, MonoGFX_COLOR_ON); // triangle
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 3, y - 3, MonoGFX_COLOR_ON);
	MonoGFX_set_pixel(x + GUI_ROW_WIDTH / 3 + 4, y - 4, MonoGFX_COLOR_ON);
}

void GUI_component_select_render(GUI_Component* component) {
	if (component->selected) {
		// MonoGFX_draw_line(component->x, component->y, component->x, component->y + 4, MonoGFX_COLOR_ON); // GUI_LEFT_PADDING = 2
		
		switch (component->type) {
			case GUI_COMPONENT_BUTTON:
			case GUI_COMPONENT_SCROLL_BUTTON:
			case GUI_COMPONENT_TOGGLE_BUTTON:
			case GUI_COMPONENT_RADIO_BUTTON:
			case GUI_COMPONENT_MENU_BUTTON: 
				MonoGFX_fill_rect(component->x - 1, component->y - 1, component->width - 1, component->height - 1, MonoGFX_COLOR_INVERT); // GUI_LEFT_PADDING = 0
				break;
			case GUI_COMPONENT_GRAPH_LABEL:
				GUI_graphLabel_select_render((GUI_GraphLabel*)component->component, component->x, component->y);
				break;
			case GUI_COMPONENT_RANGE:
				GUI_range_select_render((GUI_Range*)component->component, component->x, component->y);
				break;
			case GUI_COMPONENT_LABEL: // can't be selected
			case GUI_COMPONENT_SPRITE:
			case GUI_COMPONENT_GRAPH:
			case GUI_COMPONENT_MENU_COLUMN:
			case GUI_COMPONENT_MENU:
			case GUI_COMPONENT_SCREEN:
				break;
		}
	}
}
void GUI_component_render(GUI_Component* component) {
	if (!component->visible) {
		return;
	}
	switch (component->type) {
		case GUI_COMPONENT_GRAPH_LABEL: {
			GUI_graphLabel_render((GUI_GraphLabel*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_LABEL: {
			GUI_label_render((GUI_Label*)component->component, component->x, component->y);
		} break;
		case GUI_COMPONENT_BUTTON: {
			GUI_button_render((GUI_Button*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_SCROLL_BUTTON: {
			GUI_scrollButton_render((GUI_ScrollButton*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_RANGE: {
			GUI_range_render((GUI_Range*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_TOGGLE_BUTTON: {
			GUI_toggleButton_render((GUI_ToggleButton*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_RADIO_BUTTON: {
			GUI_radioButton_render((GUI_RadioButton*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_MENU_COLUMN: {
			GUI_menuColumn_render((GUI_MenuColumn*)component->component, component->x, component->y);
		} break;
		case GUI_COMPONENT_MENU: {
			GUI_menu_render((GUI_Menu*)component->component, component->x, component->y);
		} break;
		case GUI_COMPONENT_MENU_BUTTON: {
			GUI_menuButton_render((GUI_MenuButton*)component->component, component->x, component->y);
			GUI_component_select_render(component);
		} break;
		case GUI_COMPONENT_GRAPH: {
			GUI_graph_render((GUI_Graph*)component->component, component->x, component->y);
		} break;
		case GUI_COMPONENT_SCREEN: {
			GUI_screen_render((GUI_Screen*)component->component, component->x, component->y);
		} break;
		case GUI_COMPONENT_SPRITE: {
			GUI_sprite_render((GUI_Sprite*)component->component, component->x, component->y);
		} break;
	}
}
void GUI_component_set_tabNext(GUI_Component* component, GUI_Component* tabNext) {
	component->tabNext = tabNext;
	tabNext->tabPrev = component;
}

GUI_Component* GUI_component_create(GUI_ComponentTypedef type, int32_t x, int32_t y, void* comp) {
	GUI_Component* component = (GUI_Component*)malloc(sizeof(GUI_Component));
	
	component->type = type;
	component->x = x;
	component->y = y;
	component->width = GUI_ROW_WIDTH;
	component->height = GUI_ROW_HEIGHT;
	component->component = comp;
	component->id = GUI_idCounter;
	component->visible = 1;
	component->selected = 0;
	component->tabNext = NULL;
	component->tabPrev = NULL;
	GUI_idCounter ++;
	
	return component;
}

GUI_Label* GUI_label_create(char* text, uint32_t fontID) {
	GUI_Label* label = (GUI_Label*)malloc(sizeof(GUI_Label));
	
	if (strlen(text) >= GUI_LABEL_MAX_LEN - 1) {
		Error_Handler();
	}
	strcpy(label->value, text);
	label->formatter = formatSimpleText;
	label->font = fontID;
	label->align = GUI_TEXT_ALIGN_LEFT;
	
	return label;
}
void GUI_label_render(GUI_Label* label, int32_t x, int32_t y) {
	char tmp[GUI_LABEL_MAX_LEN];
	
	GUI_label_getText(label, tmp);
	
	GUI_write_string(x, y, tmp, label->font, label->align, GUI_TEXT_DIRECTION_HORIZONTAL, MonoGFX_COLOR_ON);
}
void GUI_label_getText(GUI_Label* label, char* out) {
	label->formatter(out, (void*)label->value);
}

GUI_Button* GUI_button_create(char* text, uint32_t fontID, GUI_CallbackTypedef onClick) {
	GUI_Button* button = (GUI_Button*)malloc(sizeof(GUI_Button));
	
	button->label = GUI_label_create(text, fontID);
	button->onClick = onClick;
	
	return button;
}
void GUI_button_render(GUI_Button* button, int32_t x, int32_t y) {
	GUI_label_render(button->label, x + GUI_LEFT_PADDING, y);
}

GUI_ScrollButton* GUI_scrollButton_create(char* text, uint32_t fontID, float min, float max, float val, GUI_CallbackTypedef onScroll) {
	GUI_ScrollButton* scrollButton = (GUI_ScrollButton*)malloc(sizeof(GUI_ScrollButton));
	
	if (strlen(text) >= GUI_LABEL_MAX_LEN - 2) { // 1 char will be appended
		Error_Handler();
	}
	char tmp[GUI_LABEL_MAX_LEN];
	strcpy(tmp, text);
	strcat(tmp, GUI_ROTATE_CHAR);
	scrollButton->button = GUI_button_create(tmp, fontID, NULL);
	scrollButton->onScroll = onScroll;
	scrollButton->min = min;
	scrollButton->max = max;
	scrollButton->largeStep = 5;
	scrollButton->smallStep = 1;
	scrollButton->value = val;
	scrollButton->labelIsValue = 0;
	scrollButton->panelEnabled = 0;
	scrollButton->lastScrollTime = 0;
	
	GUI_scrollButton_clamp_value(scrollButton);
	
	return scrollButton;
}
void GUI_scrollButton_render(GUI_ScrollButton* scrollButton, int32_t x, int32_t y) {
	if (scrollButton->labelIsValue) {
		*((float*)scrollButton->button->label->value) = scrollButton->value;
	}
	GUI_button_render(scrollButton->button, x, y);
	
	if (scrollButton->panelEnabled && (HAL_GetTick() - scrollButton->lastScrollTime) < GUI_SCROLL_BUTTON_PANEL_SHOW_TIMEOUT && scrollButton->lastScrollTime) {
		MonoGFX_fill_round_rect(x - 2, y - 11, GUI_ROW_WIDTH + 1, 7, 1, MonoGFX_COLOR_OFF);
		MonoGFX_draw_round_rect(x - 2, y - 11, GUI_ROW_WIDTH + 1, 7, 1, MonoGFX_COLOR_ON);
		GUI_drawPanelTriangle(x, y, MonoGFX_COLOR_OFF);
		
		uint32_t scrollWidth = GUI_ROW_WIDTH - 4;
		float fullScale = (float)(scrollButton->max - scrollButton->min);
		float dynamicValue = (float)(scrollButton->value - scrollButton->min);
		uint32_t position = (dynamicValue / fullScale) * scrollWidth;
		
		MonoGFX_draw_line(x, y - 8, x + scrollWidth, y - 8, MonoGFX_COLOR_ON);
		MonoGFX_draw_line(x + position, y - 9, x + position, y - 7, MonoGFX_COLOR_ON);
	}
}
void GUI_scrollButton_clamp_value(GUI_ScrollButton* scrollButton) {
	if (scrollButton->value > scrollButton->max) {
		scrollButton->value = scrollButton->max;
	}
	if (scrollButton->value < scrollButton->min) {
		scrollButton->value = scrollButton->min;
	}
}
void GUI_scrollButton_scroll(GUI_ScrollButton* scrollButton, int16_t delta, uint8_t largeStep) {
	scrollButton->lastScrollTime = HAL_GetTick();
	scrollButton->value += (largeStep ? scrollButton->largeStep : scrollButton->smallStep) * delta;
	GUI_scrollButton_clamp_value(scrollButton);
	
	if (scrollButton->onScroll) {
		scrollButton->onScroll(scrollButton);
	}
}


GUI_Range* GUI_range_create(char* text, uint32_t fontID, float min, float max, float val, GUI_CallbackTypedef onScroll, uint32_t size, GUI_StringFormatterTypedef valueFromatter) {
	GUI_Range* range = (GUI_Range*)malloc(sizeof(GUI_Range));
	
	range->scrollButton = GUI_scrollButton_create(text, fontID, min, max, val, onScroll);
	range->valueLabel = GUI_label_create("", fontID);
	range->valueLabel->formatter = valueFromatter;
	range->textAlign = GUI_TEXT_ALIGN_LEFT;
	range->vertical = 0;
	range->size = size;
	
	return range;
}
void GUI_range_render(GUI_Range* range, int32_t x, int32_t y) {
	float fullScale = (float)(range->scrollButton->max - range->scrollButton->min);
	float dynamicValue = (float)(range->scrollButton->value - range->scrollButton->min);
	uint32_t position = (dynamicValue / fullScale) * range->size;
	
	*((float*)range->valueLabel->value) = range->scrollButton->value;
	if (range->vertical) {
		if (range->valueLabel->align == GUI_TEXT_ALIGN_LEFT) {
			GUI_label_render(range->valueLabel, x + 4, y + GUI_ROW_HEIGHT * 2); // TODO: SIZE MANAGER y should be center aligned
			GUI_scrollButton_render(range->scrollButton, x + 4, y + GUI_ROW_HEIGHT); // TODO: y should be center aligned
		} else if (range->valueLabel->align == GUI_TEXT_ALIGN_RIGHT) {
			GUI_label_render(range->valueLabel, x - 2, y + GUI_ROW_HEIGHT * 2); // TODO: SIZE MANAGER y should be center aligned
			GUI_scrollButton_render(range->scrollButton, x - 2, y + GUI_ROW_HEIGHT); // TODO: y should be center aligned
		}
		MonoGFX_draw_line(x, y, x, y + range->size, MonoGFX_COLOR_ON);
		MonoGFX_draw_line(x - 1, y + range->size - position, x + 1, y + range->size - position, MonoGFX_COLOR_ON);
	} else {
		if (range->valueLabel->align == GUI_TEXT_ALIGN_LEFT) {
			GUI_label_render(range->valueLabel, x + 2, y - GUI_ROW_HEIGHT + 1); // TODO: SIZE MANAGER x should be center aligned
		} else if (range->valueLabel->align == GUI_TEXT_ALIGN_RIGHT) {
			GUI_label_render(range->valueLabel, x + range->size + 2, y + 4); // TODO: SIZE MANAGER x should be center aligned
		}
		GUI_scrollButton_render(range->scrollButton, x + 2, y + 4); // TODO: x should be center aligned
		
		MonoGFX_draw_line(x, y, x + range->size, y, MonoGFX_COLOR_ON);
		MonoGFX_draw_line(x + position, y - 1, x + position, y + 1, MonoGFX_COLOR_ON);
	}
}

void GUI_range_select_render(GUI_Range* range, int32_t x, int32_t y) {
	if (range->vertical) {
		if (range->valueLabel->align == GUI_TEXT_ALIGN_LEFT) {
			MonoGFX_fill_rect(x + 3, y + GUI_ROW_HEIGHT - 1, GUI_ROW_WIDTH - 1, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_INVERT);
		} else if (range->valueLabel->align == GUI_TEXT_ALIGN_RIGHT) {
			MonoGFX_fill_rect(x - GUI_ROW_WIDTH - 1, y + GUI_ROW_HEIGHT - 1, GUI_ROW_WIDTH - 1, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_INVERT);
		}
	} else {
		MonoGFX_fill_rect(x + 1, y + 3, GUI_ROW_WIDTH - 1, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_INVERT);
	}
}


GUI_ToggleButton* GUI_toggleButton_create(char* defaultText, uint32_t fontID, GUI_CallbackTypedef onClick) {
	GUI_ToggleButton* toggleButton = (GUI_ToggleButton*)malloc(sizeof(GUI_ToggleButton));
	
	if (strlen(defaultText) >= GUI_LABEL_MAX_LEN - 1) {
		Error_Handler();
	}
	
	toggleButton->button = GUI_button_create(defaultText, fontID, onClick);
	toggleButton->checked = 0;
	toggleButton->showCheckbox = 1;
	strcpy(toggleButton->defaultText, defaultText);
	strcpy(toggleButton->checkedText, defaultText);
	
	return toggleButton;
}
void GUI_toggleButton_render(GUI_ToggleButton* toggleButton, int32_t x, int32_t y) {
	if (toggleButton->checked) {
		strcpy(toggleButton->button->label->value, toggleButton->checkedText);
	} else {
		strcpy(toggleButton->button->label->value, toggleButton->defaultText);
	}
	if (toggleButton->showCheckbox) {
		if (toggleButton->checked) {
			// MonoGFX_fill_rect(x + GUI_LEFT_PADDING, y, 5, 5, MonoGFX_COLOR_ON); // TODO: SIZE MANAGER x should be right aligned
			MonoGFX_set_pixel(x + GUI_LEFT_PADDING + 1, y + 1, MonoGFX_COLOR_ON); // top two pixels
			MonoGFX_set_pixel(x + GUI_LEFT_PADDING + 3, y + 1, MonoGFX_COLOR_ON); // top two pixels
			MonoGFX_set_pixel(x + GUI_LEFT_PADDING + 2, y + 2, MonoGFX_COLOR_ON); // center pixel
			MonoGFX_set_pixel(x + GUI_LEFT_PADDING + 1, y + 3, MonoGFX_COLOR_ON); // bottom two pixels
			MonoGFX_set_pixel(x + GUI_LEFT_PADDING + 3, y + 3, MonoGFX_COLOR_ON); // bottom two pixels
		}
		MonoGFX_draw_rect(x + GUI_LEFT_PADDING, y, 5, 5, MonoGFX_COLOR_ON); // TODO: SIZE MANAGER x should be right aligned
		GUI_button_render(toggleButton->button, x + 6, y);
	} else {
		GUI_button_render(toggleButton->button, x + 2, y);
	}
	
}
void GUI_toggleButton_click(GUI_ToggleButton* toggleButton) {
	toggleButton->checked = !toggleButton->checked;
	if (toggleButton->button->onClick != NULL) {
		toggleButton->button->onClick(toggleButton);
	}
}

GUI_RadioButton* GUI_radioButton_create(char* defaultText, uint32_t fontID, GUI_CallbackTypedef onClick) {
	GUI_RadioButton* radioButton = (GUI_RadioButton*)malloc(sizeof(GUI_RadioButton));
	
	radioButton->toggleButton = GUI_toggleButton_create(defaultText, fontID, onClick);
	radioButton->toggleButton->showCheckbox = 0;
	radioButton->nextRadioButton = radioButton;
	
	return radioButton;
}
void GUI_radioButton_render(GUI_RadioButton* radioButton, int32_t x, int32_t y) {
	GUI_toggleButton_render(radioButton->toggleButton, x + 4, y);
	MonoGFX_draw_round_rect(x + GUI_LEFT_PADDING, y, 5, 5, 1, MonoGFX_COLOR_ON); // TODO: SIZE MANAGER x should be right aligned
	if (radioButton->toggleButton->checked) {
		MonoGFX_draw_line(x + GUI_LEFT_PADDING, y + 2, x + 3 + GUI_LEFT_PADDING, y + 2, MonoGFX_COLOR_ON); // TODO: SIZE MANAGER x should be right aligned
		MonoGFX_draw_line(x + 2 + GUI_LEFT_PADDING, y, x + 2 + GUI_LEFT_PADDING, y + 4, MonoGFX_COLOR_ON); // TODO: SIZE MANAGER x should be right aligned
	}
}
void GUI_radioButton_click(GUI_RadioButton* radioButton) {
	GUI_RadioButton* tmp = radioButton;
	do {
		tmp->toggleButton->checked = 0;
		tmp = tmp->nextRadioButton;
	} while (tmp != radioButton);
	
	radioButton->toggleButton->checked = 1;
	if (radioButton->toggleButton->button->onClick != NULL) {
		radioButton->toggleButton->button->onClick(radioButton);
	}
}
GUI_RadioButton* GUI_radioButton_get_selected(GUI_RadioButton* radioButton) {
	GUI_RadioButton* tmp = radioButton;
	do {
		if (tmp->toggleButton->checked) {
			return tmp;
		}
		tmp = tmp->nextRadioButton;
	} while (tmp != radioButton);
	
	return NULL;
}

GUI_MenuColumn* GUI_menuColumn_create() {
	GUI_MenuColumn* menuColumn = (GUI_MenuColumn*)malloc(sizeof(GUI_MenuColumn));
	
	menuColumn->nextCol = menuColumn; // circular list
	menuColumn->numRows = 0;
	
	return menuColumn;
}
int32_t GUI_menuColumn_add_component(GUI_MenuColumn* menuColumn, GUI_Component* component) {
	if (menuColumn->numRows < GUI_MENU_COLUMN_MAX_ROWS) {
		menuColumn->rows[menuColumn->numRows] = component;
		menuColumn->numRows++;
		
		if (menuColumn->numRows > 1) { // add to end of list
			component->tabPrev = menuColumn->rows[menuColumn->numRows - 2]; // previous element
			component->tabNext = menuColumn->nextCol->rows[0]; // link to first element of next column
			
			menuColumn->rows[menuColumn->numRows - 2]->tabNext = component;
			menuColumn->nextCol->rows[0]->tabPrev = component; // link first element of next column to this
			if (component->type == GUI_COMPONENT_RADIO_BUTTON) {
				GUI_RadioButton* rb = (GUI_RadioButton*)component->component;
				GUI_RadioButton* prevRb = (GUI_RadioButton*)menuColumn->rows[menuColumn->numRows - 2]->component;
				GUI_RadioButton* firstRb = (GUI_RadioButton*)menuColumn->rows[0]->component;
				prevRb->nextRadioButton = rb;
				rb->nextRadioButton = firstRb;
			}
		} else { // only element in this column
			GUI_MenuColumn* prevCol = GUI_menuColumn_get_previous_col(menuColumn);
			component->tabPrev = prevCol->rows[prevCol->numRows - 1]; // what if empty previous column?
			prevCol->rows[prevCol->numRows - 1]->tabNext = component;
			component->tabNext = menuColumn->nextCol->rows[0];
			menuColumn->nextCol->rows[0]->tabPrev = component;
		}
		
		
		return (menuColumn->numRows - 1);
	}
	return -1;
}
GUI_MenuColumn* GUI_menuColumn_get_previous_col(GUI_MenuColumn* menuColumn) {
	GUI_MenuColumn* tmp = menuColumn;
	
	if (tmp->nextCol == menuColumn) { // only one column
		return tmp;
	}
	while(tmp->nextCol != menuColumn) {
		tmp = tmp->nextCol;
	}
	
	return tmp;
}

void GUI_menuColumn_render(GUI_MenuColumn* menuColumn, int32_t x, int32_t y) {
	MonoGFX_fill_round_rect(x - 2, y - 2, GUI_ROW_WIDTH + 1, menuColumn->numRows * GUI_ROW_HEIGHT + 1, 1, MonoGFX_COLOR_OFF);
	MonoGFX_draw_round_rect(x - 2, y - 2, GUI_ROW_WIDTH + 1, menuColumn->numRows * GUI_ROW_HEIGHT + 1, 1, MonoGFX_COLOR_ON);
	for (uint32_t i = 0; i < menuColumn->numRows; i ++) {
		menuColumn->rows[i]->x = x; // TODO: SIZE MANAGER
		menuColumn->rows[i]->y = y + i * GUI_ROW_HEIGHT; // TODO: SIZE MANAGER
		GUI_component_render(menuColumn->rows[i]);
		MonoGFX_draw_line(x - 1, y + (i + 1) * GUI_ROW_HEIGHT - 2, x + GUI_ROW_WIDTH - 3, y + (i + 1) * GUI_ROW_HEIGHT - 2, MonoGFX_COLOR_ON); // row divider
	}
}

GUI_Menu* GUI_menu_create() {
	GUI_Menu* menu = (GUI_Menu*)malloc(sizeof(GUI_Menu));
	
	menu->columns = NULL;
	
	return menu;
}
void GUI_menu_add_column(GUI_Menu* menu, GUI_MenuColumn* col) {
	GUI_MenuColumn** tmp = &menu->columns;
	
	if (*tmp == NULL) {
		*tmp = col;
		return;
	}
	while ((*tmp)->nextCol != menu->columns) { // find last column
		tmp = &(*tmp)->nextCol;
	}
	(*tmp)->nextCol = col;
	if ((*tmp)->numRows && col->numRows) {
		(*tmp)->rows[(*tmp)->numRows - 1]->tabNext = col->rows[0];
		col->rows[0]->tabPrev = (*tmp)->rows[(*tmp)->numRows - 1];
	}
	col->nextCol = menu->columns; // circular list
	if (menu->columns->numRows && col->numRows) {
		menu->columns->rows[0]->tabPrev = col->rows[col->numRows - 1];
		col->rows[col->numRows - 1]->tabNext = menu->columns->rows[0];
	}
}



void GUI_menu_render(GUI_Menu* menu, int32_t x, int32_t y) {
	GUI_MenuColumn* tmp = menu->columns;
	if (tmp == NULL) {
		return;
	}
	
	uint32_t maxNumRows = 0;
	
	do {
		if (maxNumRows < tmp->numRows) {
			maxNumRows = tmp->numRows;
		}
		tmp = tmp->nextCol;
	} while(tmp != menu->columns);
	tmp = menu->columns;
	
	uint32_t numCols = GUI_menu_get_num_columns(menu);
	uint32_t i = 0;
	int32_t cx, cy;
	do {
		cx = x - (numCols - 1) * GUI_ROW_WIDTH + i * GUI_ROW_WIDTH;
		cy = y - GUI_ROW_HEIGHT * maxNumRows - 3;
		if (i > 0) {
			MonoGFX_set_pixel(cx - 2, cy - 2, MonoGFX_COLOR_ON); // makes top of menu straight line, hiding round rect roundness of columns
		}
		GUI_menuColumn_render(tmp, cx, cy);
		i ++;
		tmp = tmp->nextCol;
	} while(tmp != menu->columns);
	
	uint8_t triangleFill = MonoGFX_COLOR_OFF; // menu triangle should be filled, when last element is selected
	tmp = GUI_menuColumn_get_previous_col(menu->columns); // last column
	if (tmp->numRows) {
		if (tmp->rows[tmp->numRows - 1]->selected) {
			triangleFill = MonoGFX_COLOR_ON;
		}
	}
	
	GUI_drawPanelTriangle(x, y, triangleFill);
}
uint32_t GUI_menu_get_num_columns(GUI_Menu* menu) {
	GUI_MenuColumn* tmp = menu->columns;
	if (tmp == NULL) {
		return 0;
	}
	uint32_t n = 0;
	do {
		n ++;
		tmp = tmp->nextCol;
	} while(tmp != menu->columns);
	return n;
}

GUI_MenuButton* GUI_menuButton_create(char* defaultText, uint32_t fontID, GUI_CallbackTypedef onClick, GUI_Menu* menu) {
	GUI_MenuButton* menuButton = (GUI_MenuButton*)malloc(sizeof(GUI_MenuButton));
	
	if (strlen(defaultText) >= GUI_LABEL_MAX_LEN - 2) { // 1 char will be appended
		Error_Handler();
	}
	
	char tmp[GUI_LABEL_MAX_LEN];
	strcpy(tmp, defaultText);
	strcat(tmp,  "^");
	
	menuButton->toggleButton = GUI_toggleButton_create(tmp, fontID, onClick);
	menuButton->toggleButton->showCheckbox = 0;
	
	strcpy(menuButton->toggleButton->checkedText, defaultText);
	strcat(menuButton->toggleButton->checkedText, GUI_DOWN_CHAR);
	
	menuButton->menu = menu;
	
	return menuButton;
}
void GUI_menuButton_render(GUI_MenuButton* menuButton, int32_t x, int32_t y) {
	GUI_toggleButton_render(menuButton->toggleButton, x, y);
	if (menuButton->toggleButton->checked) {
		GUI_menu_render(menuButton->menu, x, y); // TODO: SIZE MANAGER, menu is bottom aligned
	}
}
void GUI_menuButton_click(GUI_MenuButton* menuButton) {
	uint8_t prevState = menuButton->toggleButton->checked;
	GUI_close_all_menus(); // this would close this menu too, which means, we couldn't toggle an open menu by pressing it's menuButton
	menuButton->toggleButton->checked = prevState; // cheap workaround
	
	GUI_toggleButton_click(menuButton->toggleButton);
	if (menuButton->toggleButton->checked && menuButton->menu->columns != NULL) { // select first item in menu when it's selected
		if (menuButton->menu->columns->numRows) {
			GUI_select_component(menuButton->menu->columns->rows[0]);
		}
	}
}

GUI_GraphLabel* GUI_graphLabel_create(char* text, MonoGFX_LineStyleTypedef lineStyle, uint8_t floatRight, uint8_t vertical, uint32_t fontID, GUI_CallbackTypedef onScroll) {
	GUI_GraphLabel* graphLabel = (GUI_GraphLabel*)malloc(sizeof(GUI_GraphLabel));
	
	graphLabel->scrollButton = GUI_scrollButton_create(text, fontID, -1000000.0f, 1000000.0f, 0.0f, onScroll);
	graphLabel->nextLabel = NULL;
	graphLabel->lineStyle = lineStyle;
	graphLabel->rightChannel = floatRight;
	graphLabel->vertical = vertical;
	
	return graphLabel;
}
void GUI_graphLabel_render(GUI_GraphLabel* graphLabel, int32_t x, int32_t y) {
	char tmp[GUI_LABEL_MAX_LEN];
	GUI_label_getText(graphLabel->scrollButton->button->label, tmp);
	uint32_t strw = GUI_get_string_width(tmp, graphLabel->scrollButton->button->label->font);
	
	int32_t labelHeightOffset = 2;
	int32_t labelWidthOffset = strw / 2;
	
	if (graphLabel->vertical) {
		y -= labelHeightOffset;
		
		MonoGFX_draw_line_style(x, y + 2, x + graphLabel->ownerGraph->w, y + 2, graphLabel->lineStyle, MonoGFX_COLOR_ON); // horizontal line
		
		if (graphLabel->rightChannel) {
			x += graphLabel->ownerGraph->w - strw - 1;
		}
		
		MonoGFX_fill_rect(x, y - 1, strw + 1, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_OFF); // background
		MonoGFX_draw_line(x, y - 2, x + strw, y - 2, MonoGFX_COLOR_ON); // top line
		MonoGFX_draw_line(x, y + GUI_ROW_HEIGHT - 2, x + strw, y + GUI_ROW_HEIGHT - 2, MonoGFX_COLOR_ON); // bottom line
		if (graphLabel->rightChannel) {
			MonoGFX_set_pixel(x - 1, y - 1, MonoGFX_COLOR_ON); // left top pixel
			MonoGFX_set_pixel(x - 1, y, MonoGFX_COLOR_ON); // left top pixel
			
			MonoGFX_set_pixel(x - 1, y + 1, MonoGFX_COLOR_OFF); // left center pixels clear
			MonoGFX_set_pixel(x - 1, y + 2, MonoGFX_COLOR_OFF); // left center pixels clear
			MonoGFX_set_pixel(x - 1, y + 3, MonoGFX_COLOR_OFF); // left center pixels clear
			MonoGFX_set_pixel(x - 2, y + 2, MonoGFX_COLOR_OFF); // left center center pixel clear
			
			MonoGFX_set_pixel(x - 2, y + 1, MonoGFX_COLOR_ON); // left center top pixel
			MonoGFX_set_pixel(x - 3, y + 2, MonoGFX_COLOR_ON); // left center center pixel
			MonoGFX_set_pixel(x - 2, y + 3, MonoGFX_COLOR_ON); // left center bottom pixel
			
			MonoGFX_set_pixel(x - 1, y + 4, MonoGFX_COLOR_ON); // left bottom pixel
			MonoGFX_set_pixel(x - 1, y + 5, MonoGFX_COLOR_ON); // left bottom pixel
		} else {
			MonoGFX_set_pixel(x + strw + 1, y - 1, MonoGFX_COLOR_ON); // right top pixel
			MonoGFX_set_pixel(x + strw + 1, y, MonoGFX_COLOR_ON); // right top pixel
			
			MonoGFX_set_pixel(x + strw + 1, y + 1, MonoGFX_COLOR_OFF); // right center pixels clear
			MonoGFX_set_pixel(x + strw + 1, y + 2, MonoGFX_COLOR_OFF); // right center pixels clear
			MonoGFX_set_pixel(x + strw + 1, y + 3, MonoGFX_COLOR_OFF); // right center pixels clear
			MonoGFX_set_pixel(x + strw + 2, y + 2, MonoGFX_COLOR_OFF); // right center center pixel clear
			
			MonoGFX_set_pixel(x + strw + 2, y + 1, MonoGFX_COLOR_ON); // right center top pixel
			MonoGFX_set_pixel(x + strw + 3, y + 2, MonoGFX_COLOR_ON); // right center center pixel
			MonoGFX_set_pixel(x + strw + 2, y + 3, MonoGFX_COLOR_ON); // right center bottom pixel
			
			MonoGFX_set_pixel(x + strw + 1, y + 4, MonoGFX_COLOR_ON); // right bottom pixel
			MonoGFX_set_pixel(x + strw + 1, y + 5, MonoGFX_COLOR_ON); // right bottom pixel
		}
	} else {
		uint32_t lineHeight = GUI_ROW_HEIGHT;
		MonoGFX_draw_line_style(x, y + lineHeight, x, y + graphLabel->ownerGraph->h, graphLabel->lineStyle, MonoGFX_COLOR_ON); // vertical line
		
		x -= (labelWidthOffset < 2 ? 2 : labelWidthOffset); // minimum width offset is 2 because we have to draw triangle, even if there is no text
		
		MonoGFX_fill_rect(x, y, strw + 1, lineHeight - 1, MonoGFX_COLOR_OFF); // background
		MonoGFX_draw_line(x + strw + 1, y, x + strw + 1, y + lineHeight - 2, MonoGFX_COLOR_ON); // right line
		MonoGFX_draw_line(x - 1, y, x - 1, y + lineHeight - 2, MonoGFX_COLOR_ON); // left line
		MonoGFX_draw_line(x, y + lineHeight - 1, x + strw, y + lineHeight - 1, MonoGFX_COLOR_ON); // bottom line
		
		MonoGFX_set_pixel(x + strw / 2 - 1, y + lineHeight - 1, MonoGFX_COLOR_OFF); // triangle clear
		MonoGFX_set_pixel(x + strw / 2, y + lineHeight - 1, MonoGFX_COLOR_OFF); // triangle clear
		MonoGFX_set_pixel(x + strw / 2 + 1, y + lineHeight - 1, MonoGFX_COLOR_OFF); // triangle clear
		
		MonoGFX_set_pixel(x + strw / 2 - 1, y + lineHeight, MonoGFX_COLOR_ON); // center left pixel
		MonoGFX_set_pixel(x + strw / 2, y + lineHeight, MonoGFX_COLOR_OFF); // triangle center clear
		MonoGFX_set_pixel(x + strw / 2, y + lineHeight + 1, MonoGFX_COLOR_ON); // center pixel
		MonoGFX_set_pixel(x + strw / 2 + 1, y + lineHeight, MonoGFX_COLOR_ON); // center right pixel
		
		y++;
	}
	
	GUI_label_render(graphLabel->scrollButton->button->label, x + 1, y); // text height is 5pixels, half of text height is 2.5f
}
void GUI_graphLabel_select_render(GUI_GraphLabel* graphLabel, int32_t x, int32_t y) {
	char tmp[GUI_LABEL_MAX_LEN];
	GUI_label_getText(graphLabel->scrollButton->button->label, tmp);
	uint32_t strw = GUI_get_string_width(tmp, graphLabel->scrollButton->button->label->font);
	
	int32_t labelHeightOffset = 2;
	int32_t labelWidthOffset = strw / 2;
	
	if (graphLabel->vertical) {
		y -= labelHeightOffset;
		
		if (graphLabel->rightChannel) {
			x += graphLabel->ownerGraph->w - strw - 1;
		}
		
		MonoGFX_fill_rect(x, y - 1, strw + 1, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_INVERT); // background
		
		if (graphLabel->rightChannel) {
			MonoGFX_set_pixel(x - 1, y + 1, MonoGFX_COLOR_INVERT); // left center pixels clear
			MonoGFX_set_pixel(x - 1, y + 2, MonoGFX_COLOR_INVERT); // left center pixels clear
			MonoGFX_set_pixel(x - 1, y + 3, MonoGFX_COLOR_INVERT); // left center pixels clear
			MonoGFX_set_pixel(x - 2, y + 2, MonoGFX_COLOR_INVERT); // left center center pixel clear
		} else {
			MonoGFX_set_pixel(x + strw + 1, y + 1, MonoGFX_COLOR_INVERT); // right center pixels clear
			MonoGFX_set_pixel(x + strw + 1, y + 2, MonoGFX_COLOR_INVERT); // right center pixels clear
			MonoGFX_set_pixel(x + strw + 1, y + 3, MonoGFX_COLOR_INVERT); // right center pixels clear
			MonoGFX_set_pixel(x + strw + 2, y + 2, MonoGFX_COLOR_INVERT); // right center center pixel clear
		}
	} else {
		uint32_t lineHeight = GUI_ROW_HEIGHT;
		MonoGFX_draw_line_style(x, y + lineHeight, x, y + graphLabel->ownerGraph->h, graphLabel->lineStyle, MonoGFX_COLOR_ON); // vertical line
		
		x -= (labelWidthOffset < 2 ? 2 : labelWidthOffset); // minimum width offset is 2 because we have to draw triangle, even if there is no text
		
		MonoGFX_fill_rect(x, y, strw + 1, lineHeight - 1, MonoGFX_COLOR_INVERT); // background
		
		MonoGFX_set_pixel(x + strw / 2 - 1, y + lineHeight - 1, MonoGFX_COLOR_INVERT); // triangle clear
		MonoGFX_set_pixel(x + strw / 2, y + lineHeight - 1, MonoGFX_COLOR_INVERT); // triangle clear
		MonoGFX_set_pixel(x + strw / 2 + 1, y + lineHeight - 1, MonoGFX_COLOR_INVERT); // triangle clear
		
		MonoGFX_set_pixel(x + strw / 2, y + lineHeight, MonoGFX_COLOR_INVERT); // triangle center clear
		
		y++;
	}
}

GUI_Graph* GUI_graph_create(uint32_t w, uint32_t h) {
	GUI_Graph* graph = (GUI_Graph*)malloc(sizeof(GUI_Graph));
	
	graph->w = w;
	graph->h = h;
	
	GUI_ScrollButton* hDivScrollButton = GUI_scrollButton_create("", 0, 0.0f, 10.0f, 5.0f, NULL);
	graph->hDivScrollButtonComponent = GUI_component_create(GUI_COMPONENT_SCROLL_BUTTON, 0, 0, hDivScrollButton);
	graph->hDivScrollButtonComponent->width = 20;
	hDivScrollButton->button->label->formatter = formatTime;
	hDivScrollButton->labelIsValue = 1;
	
	GUI_ScrollButton* vDiv1ScrollButton = GUI_scrollButton_create("", 0, 0.0f, 10.0f, 5.0f, NULL);
	graph->vDiv1ScrollButtonComponent = GUI_component_create(GUI_COMPONENT_SCROLL_BUTTON, 0, 0, vDiv1ScrollButton);
	graph->vDiv1ScrollButtonComponent->width = 20;
	vDiv1ScrollButton->button->label->formatter = formatVoltage;
	vDiv1ScrollButton->labelIsValue = 1;
	
	GUI_ScrollButton* vDiv2ScrollButton = GUI_scrollButton_create("", 0, 0.0f, 10.0f, 5.0f, NULL);
	graph->vDiv2ScrollButtonComponent = GUI_component_create(GUI_COMPONENT_SCROLL_BUTTON, 0, 0, vDiv2ScrollButton);
	graph->vDiv2ScrollButtonComponent->width = 20;
	vDiv2ScrollButton->button->label->formatter = formatVoltage;
	vDiv2ScrollButton->labelIsValue = 1;
	
	graph->mode = GUI_GRAPH_MODE_1CH_NORMAL;
	graph->dataBufferSize = 0;
	graph->labels = NULL;
	
	graph->vOffset1GraphLabel = NULL;
	graph->vOffset2GraphLabel = NULL;
	graph->hOffsetGraphLabel = NULL;
	
	graph->Fs_Hz = 1.0f;
	graph->data_1 = NULL;
	graph->data_2 = NULL;
	
	return graph;
}
void calculateLabelPosition(GUI_Graph* graph, GUI_Component* tmp, int32_t x, int32_t y, float vOffset1, float vOffset2, float hOffset, float pixelsPerVDiv1, float pixelsPerVDiv2, float pixelsPerHDiv) {
	GUI_GraphLabel* tmpGraphLabel = ((GUI_GraphLabel*)tmp->component);
	float tmpOffset = 0.0f;
	if (tmpGraphLabel->vertical) {
		tmp->x = x;
		if (tmpGraphLabel->rightChannel) {
			if (tmpGraphLabel == graph->vOffset2GraphLabel) {
				tmpOffset = 0;
			} else {
				tmpOffset = vOffset2;
			}
			tmp->y = y + graph->h - ((tmpGraphLabel->scrollButton->value + tmpOffset) * pixelsPerVDiv2);
		} else {
			if (tmpGraphLabel == graph->vOffset1GraphLabel) {
				tmpOffset = 0;
			} else {
				tmpOffset = vOffset1;
			}
			tmp->y = y + graph->h - ((tmpGraphLabel->scrollButton->value + tmpOffset) * pixelsPerVDiv1);
		}
	} else {
		if (graph->mode == GUI_GRAPH_MODE_XY) {
			// in XY mode, vOffset1 is x offset
			tmp->x = x + ((tmpGraphLabel->scrollButton->value) * pixelsPerVDiv1);
			tmp->y = y;
		} else {
			if (tmpGraphLabel == graph->hOffsetGraphLabel) {
				tmpOffset = 0;
			} else {
				tmpOffset = hOffset;
			}
			tmp->x = x + (graph->w / 2 - 1) - ((tmpGraphLabel->scrollButton->value + tmpOffset) * pixelsPerHDiv);
			tmp->y = y;
		}
	}
}
void GUI_graph_render(GUI_Graph* graph, int32_t x, int32_t y) {
	// TODO: math support (add, subtract, multiply, [...] channels)
	// TODO: cursors support in graph
	uint32_t numVDots = 4;
	uint32_t numHDots = 6;
	float pixelsPerVDiv1 = ((float)graph->h / (float)numVDots) / ((GUI_ScrollButton*)graph->vDiv1ScrollButtonComponent->component)->value;
	float pixelsPerVDiv2 = ((float)graph->h / (float)numVDots) / ((GUI_ScrollButton*)graph->vDiv2ScrollButtonComponent->component)->value;
	float pixelsPerHDiv = 1.0f; // see lines below, each sample has it's own pixel
	
	float Ts_s = 1.0f / graph->Fs_Hz;
	float timePerHDiv_s = Ts_s * ((float)graph->w / (float)numHDots); // each sample gets it's own pixel
	
	*((float*)((GUI_ScrollButton*)graph->hDivScrollButtonComponent->component)->button->label->value) = timePerHDiv_s;
	
	float vOffset1 = 0.0f; // in volts
	float vOffset2 = 0.0f; // in volts
	float hOffset = 0.0f;
	
	if (graph->vOffset1GraphLabel != NULL) {
		if (graph->mode == GUI_GRAPH_MODE_XY) {
			graph->vOffset1GraphLabel->vertical = 0;
		} else {
			graph->vOffset1GraphLabel->vertical = 1;
		}
		vOffset1 = graph->vOffset1GraphLabel->scrollButton->value;
	}
	if (graph->vOffset2GraphLabel != NULL) {
		vOffset2 = graph->vOffset2GraphLabel->scrollButton->value;
	}
	if (graph->hOffsetGraphLabel != NULL) {
		hOffset = graph->hOffsetGraphLabel->scrollButton->value;
	}
	
	if (graph->dataBufferSize == 0) {
		GUI_write_string(x + graph->w / 2, y + graph->h / 2, "NO DATA", 0, GUI_TEXT_ALIGN_CENTER, GUI_TEXT_DIRECTION_HORIZONTAL, MonoGFX_COLOR_ON);
	} else {
		
		for (uint32_t vDots = 0; vDots <= numVDots; vDots ++) {
			for (uint32_t hDots = 0; hDots <= numHDots; hDots ++) {
				MonoGFX_set_pixel(x + hDots * ((graph->w - 1) / (float)numHDots), y + vDots * ((graph->h) / (float)numVDots), MonoGFX_COLOR_ON);
			}
		}
		switch (graph->mode) {
			case GUI_GRAPH_MODE_1CH_NORMAL:
			case GUI_GRAPH_MODE_2CH_NORMAL:
			case GUI_GRAPH_MODE_12CH_NORMAL: {
				int32_t currX;
				uint32_t idx;
				uint32_t graphAreaWidth = graph->w;
				int32_t idxOffset = (graph->dataBufferSize >> 1) - (graphAreaWidth >> 1) + hOffset; // buffer center is to be centered on screen
				float currVal, nextVal;
				
				if (graph->mode == GUI_GRAPH_MODE_1CH_NORMAL || graph->mode == GUI_GRAPH_MODE_12CH_NORMAL) {
					for (uint32_t i = 0; i < graphAreaWidth; i ++) {
						idx = i + idxOffset;
						currX = x + i;
						currVal = (graph->data_1[idx] + vOffset1) * pixelsPerVDiv1;
						nextVal = (graph->data_1[idx + 1] + vOffset1) * pixelsPerVDiv1; // 1V / vDiv
						
						MonoGFX_draw_line(currX, y + graph->h - currVal, currX + 1, y + graph->h - nextVal, MonoGFX_COLOR_ON);
					}
				}
				
				if (graph->mode == GUI_GRAPH_MODE_2CH_NORMAL || graph->mode == GUI_GRAPH_MODE_12CH_NORMAL) {
					for (uint32_t i = 0; i < graphAreaWidth; i ++) {
						idx = i + idxOffset;
						currX = x + i;
						currVal = (graph->data_2[idx] + vOffset2) * pixelsPerVDiv2;
						nextVal = (graph->data_2[idx + 1] + vOffset2) * pixelsPerVDiv2; // 1V / vDiv
						
						MonoGFX_draw_line(currX, y + graph->h - currVal, currX + 1, y + graph->h - nextVal, MonoGFX_COLOR_ON);
					}
				}
			} break;
			case GUI_GRAPH_MODE_XY: {
				int32_t currX;
				int32_t nextX;
				int32_t currY;
				int32_t nextY;
				for (uint32_t i = 0; i < graph->dataBufferSize - 1; i ++) {
					currX = (graph->data_1[i] + vOffset1) * pixelsPerVDiv1;
					nextX = (graph->data_1[i + 1] + vOffset1) * pixelsPerVDiv1; // 1V / vDiv
					
					currY = (graph->data_2[i] + vOffset2) * pixelsPerVDiv2;
					nextY = (graph->data_2[i + 1] + vOffset2) * pixelsPerVDiv2; // 1V / vDiv
					
					MonoGFX_draw_line(x + currX, y + graph->h - currY, x + nextX, y + graph->h - nextY, MonoGFX_COLOR_ON);
				}
			} break;
			case GUI_GRAPH_MODE_1CH_FFT:
			case GUI_GRAPH_MODE_2CH_FFT: {
				// TODO: FFT goes wild when no trigger is occuring (because we write to other channels buffer, and it's not updated from last time)
				// TODO: windowing function before FFT
				float* inputBuffer;
				float* fftBuffer;
				float vOffset;
				float pixelsPerVDiv;
				if (graph->mode == GUI_GRAPH_MODE_1CH_FFT) {
					inputBuffer = graph->data_1;
					fftBuffer = graph->data_2;
					vOffset = vOffset1;
					pixelsPerVDiv = pixelsPerVDiv1;
				} else {//  if (graph->mode == GUI_GRAPH_MODE_2CH_FFT) // commented to prevent 'uninitialized' warning
					inputBuffer = graph->data_2;
					fftBuffer = graph->data_1;
					vOffset = vOffset2;
					pixelsPerVDiv = pixelsPerVDiv2;
				}
				arm_rfft_fast_f32(&graph->FFTStruct, inputBuffer, fftBuffer, 0); // data_1 is input, data_2 is output
/*

The FFT of a real N-point sequence has even symmetry in the frequency domain.
The second half of the data equals the conjugate of the first half flipped in frequency.
Looking at the data, we see that we can uniquely represent the FFT using only N/2 complex numbers.
These are packed into the output array in alternating real and imaginary components: 

X = { real[0], imag[0], real[1], imag[1], real[2], imag[2] ... real[(N/2)-1], imag[(N/2)-1 } 

It happens that the first complex number (real[0], imag[0]) is actually all real.
real[0] represents the DC offset, and imag[0] should be 0.
(real[1], imag[1]) is the fundamental frequency, (real[2], imag[2]) is the first harmonic and so on. 

*/
				arm_cmplx_mag_f32(fftBuffer, fftBuffer, graph->dataBufferSize / 2);
				/*
				
Computes the magnitude of the elements of a complex data vector.
The pSrc points to the source data and pDst points to the where the result should be written.
numSamples specifies the number of complex samples in the input array and the data is stored in an interleaved fashion (real, imag, real, imag, ...).
The input array has a total of 2*numSamples values; the output array has a total of numSamples values.
The underlying algorithm is used:
for (n = 0; n < numSamples; n++) {
    pDst[n] = sqrt(pSrc[(2*n)+0]^2 + pSrc[(2*n)+1]^2);
}
				
				*/
				// (uint32_t)(40.0f + 20.0f * log10f(fftOutputBuffer[i])) // convert to dBV, 0dB at 40.0 pixels
				uint32_t idx;
				uint32_t graphAreaWidth = graph->w;
				int32_t currX;
				int32_t currVal;
				for (uint32_t i = 0; i < graphAreaWidth; i ++) {
					idx = (i + (int32_t)hOffset) & (graph->dataBufferSize - 1);
					currX = x + i;
					currVal = (int32_t)(((20.0f * log10f(fftBuffer[idx])) / 20.0f + vOffset) * pixelsPerVDiv); // convert to dBV, 0dB at vOffset1 pixels
					
					if (currVal > 100) { // clamp values so we don't enter extremely long loop on draw, in case of malformed data
						currVal = 100;
					}
					if (currVal < -100) {
						currVal = -100;
					}
					
					MonoGFX_draw_line(currX, y + graph->h, currX, y + graph->h - currVal, MonoGFX_COLOR_ON);
				}
			} break;
			case GUI_GRAPH_MODE_AMPLITUDE: {
				// TODO: bode graph
			} break;
			case GUI_GRAPH_MODE_PHASE: {
				// TODO: bode graph
			} break;
		}
	}
	
	MonoGFX_draw_line(x + 19, y, x + 19, y + GUI_ROW_HEIGHT - 2, MonoGFX_COLOR_ON); // top left box right line
	MonoGFX_draw_line(x, y + GUI_ROW_HEIGHT - 1, x + 18, y + GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_ON); // top left box bottom line
	MonoGFX_fill_rect(x, y, 19, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_OFF); // top left box background
	
	MonoGFX_draw_line(x + graph->w - 19, y, x + graph->w - 19, y + GUI_ROW_HEIGHT - 2, MonoGFX_COLOR_ON); // top right box left line
	MonoGFX_draw_line(x + graph->w, y + GUI_ROW_HEIGHT - 1, x + graph->w - 18, y + GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_ON); // top right box bottom line
	MonoGFX_fill_rect(x + graph->w - 18, y, 19, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_OFF); // top right box background
	
	graph->vDiv1ScrollButtonComponent->x = x + 1;
	graph->vDiv1ScrollButtonComponent->y = y + 1;
	GUI_component_render(graph->vDiv1ScrollButtonComponent);
	
	graph->vDiv2ScrollButtonComponent->x = x + graph->w - 17;
	graph->vDiv2ScrollButtonComponent->y = y + 1;
	GUI_component_render(graph->vDiv2ScrollButtonComponent);
	
	GUI_Component* tmp = graph->labels;
	GUI_Component* selectedLabel = NULL;
	
	do {
		GUI_GraphLabel* tmpGraphLabel = ((GUI_GraphLabel*)tmp->component);
		if (tmp->selected) {
			selectedLabel = tmp;
		} else {
			if (tmp->visible) {
				calculateLabelPosition(graph, tmp, x, y, vOffset1, vOffset2, hOffset, pixelsPerVDiv1, pixelsPerVDiv2, pixelsPerHDiv);
				GUI_component_render(tmp);
			}
		}
		tmp = tmpGraphLabel->nextLabel;
	} while(tmp != graph->labels);
	
	// draw center box after labels so it's not occluded by hScrollBtn
	MonoGFX_draw_line(x + (graph->w - 19) / 2 - 1, y, x + (graph->w - 19) / 2 - 1, y + GUI_ROW_HEIGHT - 2, MonoGFX_COLOR_ON); // top center box left line
	MonoGFX_draw_line(x + (graph->w + 19) / 2, y, x + (graph->w + 19) / 2, y + GUI_ROW_HEIGHT - 2, MonoGFX_COLOR_ON); // top center box right line
	MonoGFX_draw_line(x + (graph->w + 19) / 2 - 1, y + GUI_ROW_HEIGHT - 1, x + (graph->w - 19) / 2, y + GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_ON); // top center box bottom line
	MonoGFX_fill_rect(x + (graph->w - 19) / 2, y, 19, GUI_ROW_HEIGHT - 1, MonoGFX_COLOR_OFF); // top center box background
	
	graph->hDivScrollButtonComponent->x = x + (graph->w - 19) / 2 + 1;
	graph->hDivScrollButtonComponent->y = y + 1;
	GUI_component_render(graph->hDivScrollButtonComponent);
	
	if (selectedLabel) { // render selected label last, so it's on top
		if (selectedLabel->visible) {
			calculateLabelPosition(graph, selectedLabel, x, y, vOffset1, vOffset2, hOffset, pixelsPerVDiv1, pixelsPerVDiv2, pixelsPerHDiv);
			GUI_component_render(selectedLabel);
		}
	}
}
void GUI_graph_add_label(GUI_Graph* graph, GUI_Component* graphLabelComponent) {
	((GUI_GraphLabel*)graphLabelComponent->component)->ownerGraph = graph;
	if (graph->labels == NULL) {
		graph->labels = graphLabelComponent;
	} else {
		GUI_Component* tmp = graph->labels;
		if (((GUI_GraphLabel*)tmp->component)->nextLabel != graph->labels) { // only one label
			while(((GUI_GraphLabel*)tmp->component)->nextLabel != graph->labels) {
				tmp = ((GUI_GraphLabel*)tmp->component)->nextLabel;
			}
		}
		((GUI_GraphLabel*)tmp->component)->nextLabel = graphLabelComponent;
		((GUI_GraphLabel*)graphLabelComponent->component)->nextLabel = graph->labels;
	}
}

GUI_Screen* GUI_screen_create() {
	GUI_Screen* screen = (GUI_Screen*)malloc(sizeof(GUI_Screen));
	
	screen->numComponents = 0;
	screen->defaultTabComponent = NULL;
	
	if (GUI_num_screens == 0) {
		GUI_curr_screen = 0;
	}
	if (GUI_num_screens < GUI_MAX_NUM_SCREENS) {
		GUI_screens[GUI_num_screens] = screen;
		screen->id = GUI_num_screens;
		GUI_num_screens++;
	} else {
		Error_Handler();
	}
	
	screen->numComponents = 0;
	
	return screen;
}
int32_t GUI_screen_add_component(GUI_Screen* screen, GUI_Component* component) {
	if (screen->numComponents < GUI_SCREEN_MAX_COMPONENTS) {
		screen->components[screen->numComponents] = component;
		screen->numComponents++;
		
		return (screen->numComponents - 1);
	}
	return -1;
}
void GUI_screen_render(GUI_Screen* screen, int32_t x, int32_t y) {
	for (uint32_t i = 0; i < screen->numComponents; i ++) {
		GUI_component_render(screen->components[i]);
	}
}

GUI_Sprite* GUI_sprite_create(GUI_SpriteRenderCallbackTypedef renderCallback) {
	GUI_Sprite* sprite = (GUI_Sprite*)malloc(sizeof(GUI_Sprite));
	
	sprite->renderCallback = renderCallback;
	
	return sprite;
}
void GUI_sprite_render(GUI_Sprite* sprite, int32_t x, int32_t y) {
	sprite->renderCallback(sprite, x, y);
}



