
#ifndef __GUI_COMPONENTS_H
#define __GUI_COMPONENTS_H


#include "GUI_font.h"
#include "BSP_GLCD.h"
#include "MonoGFX.h"
#include <stdint.h>

typedef void (*GUI_CallbackTypedef)(void* caller);
typedef void (*GUI_StringFormatterTypedef)(char* out, void* param);

void formatPercentage(char* out, void* param);
void formatVoltage(char* out, void* param);
void formatFrequency(char* out, void* param);
void formatTime(char* out, void* param);
void formatSimpleText(char* out, void* param);
void formatEmpty(char* out, void* param);

typedef enum {
	GUI_COMPONENT_LABEL				= 0x00U,
	GUI_COMPONENT_BUTTON			= 0x01U,
	GUI_COMPONENT_SCROLL_BUTTON		= 0x02U,
	GUI_COMPONENT_RANGE				= 0x03U,
	GUI_COMPONENT_TOGGLE_BUTTON		= 0x04U,
	GUI_COMPONENT_RADIO_BUTTON		= 0x05U,
	GUI_COMPONENT_MENU_COLUMN		= 0x06U,
	GUI_COMPONENT_MENU				= 0x07U,
	GUI_COMPONENT_MENU_BUTTON		= 0x08U,
	GUI_COMPONENT_GRAPH				= 0x09U,
	GUI_COMPONENT_SCREEN			= 0x0AU,
	GUI_COMPONENT_SPRITE			= 0x0BU,
	GUI_COMPONENT_GRAPH_LABEL		= 0x0CU,
} GUI_ComponentTypedef;

typedef enum {
	GUI_TEXT_ALIGN_LEFT,
	GUI_TEXT_ALIGN_CENTER,
	GUI_TEXT_ALIGN_RIGHT,
	GUI_TEXT_ALIGN_TOP,
	GUI_TEXT_ALIGN_BOTTOM
} GUI_TextAlignTypedef;

typedef enum {
	GUI_TEXT_DIRECTION_HORIZONTAL,
	GUI_TEXT_DIRECTION_VERTICAL
} GUI_TextDirectionTypedef;

struct GUI_Component;
typedef struct GUI_Component {
	uint32_t id;
	int32_t x;
	int32_t y;
	uint32_t width;
	uint32_t height;
	
	uint8_t visible;
	struct GUI_Component* tabNext;
	struct GUI_Component* tabPrev; // circular two way list
	uint8_t selected; // 1 when selected (tab)
	
	GUI_ComponentTypedef type;
	void* component;
} GUI_Component;
GUI_Component* GUI_component_create(GUI_ComponentTypedef type, int32_t x, int32_t y, void* component);
void GUI_component_render(GUI_Component* component);
void GUI_component_set_tabNext(GUI_Component* component, GUI_Component* tabNext);

#define GUI_LABEL_MAX_LEN	16
typedef struct GUI_Label {
	char value[GUI_LABEL_MAX_LEN];
	GUI_StringFormatterTypedef formatter;
	GUI_TextAlignTypedef align;
	uint32_t font;
} GUI_Label;
GUI_Label* GUI_label_create(char* text, uint32_t fontID);
void GUI_label_render(GUI_Label* label, int32_t x, int32_t y);
void GUI_label_getText(GUI_Label* label, char* out);

typedef struct GUI_Button {
	GUI_CallbackTypedef onClick;
	GUI_Label* label;
} GUI_Button;
GUI_Button* GUI_button_create(char* text, uint32_t fontID, GUI_CallbackTypedef onClick);
void GUI_button_render(GUI_Button* button, int32_t x, int32_t y);

#define GUI_SCROLL_BUTTON_PANEL_SHOW_TIMEOUT	1500U	// time for panel to hide after scroll event in ms
typedef struct GUI_ScrollButton {
	GUI_Button* button;
	
	float min;
	float max;
	float smallStep;
	float largeStep;
	float value;
	uint8_t labelIsValue;
	uint8_t panelEnabled;
	uint32_t lastScrollTime;
	GUI_CallbackTypedef onScroll;
} GUI_ScrollButton;
GUI_ScrollButton* GUI_scrollButton_create(char* text, uint32_t fontID, float min, float max, float val, GUI_CallbackTypedef onScroll);
void GUI_scrollButton_render(GUI_ScrollButton* scrollButton, int32_t x, int32_t y);
void GUI_scrollButton_clamp_value(GUI_ScrollButton* scrollButton);
void GUI_scrollButton_scroll(GUI_ScrollButton* scrollButton, int16_t delta, uint8_t largeStep);

typedef struct GUI_Range {
	GUI_ScrollButton* scrollButton;
	GUI_Label* valueLabel;
	GUI_TextAlignTypedef textAlign;
	
	uint32_t size; // width or height in pixels
	uint8_t vertical; // 1 for vertical, 0 for horizontal
	// uint8_t labelAlign; // left or right
} GUI_Range;
GUI_Range* GUI_range_create(char* text, uint32_t fontID, float min, float max, float val, GUI_CallbackTypedef onScroll, uint32_t size, GUI_StringFormatterTypedef valueFromatter);
void GUI_range_render(GUI_Range* range, int32_t x, int32_t y);
void GUI_range_select_render(GUI_Range* range, int32_t x, int32_t y);

typedef struct GUI_ToggleButton {
	GUI_Button* button;
	
	char defaultText[GUI_LABEL_MAX_LEN]; // label text when not checked
	char checkedText[GUI_LABEL_MAX_LEN]; // label text when checked
	uint8_t checked;
	uint8_t showCheckbox; // 1 to show box on right
} GUI_ToggleButton;
GUI_ToggleButton* GUI_toggleButton_create(char* defaultText, uint32_t fontID, GUI_CallbackTypedef onClick);
void GUI_toggleButton_render(GUI_ToggleButton* toggleButton, int32_t x, int32_t y);
void GUI_toggleButton_click(GUI_ToggleButton* toggleButton);

struct GUI_RadioButton;
typedef struct GUI_RadioButton {
	GUI_ToggleButton* toggleButton;
	
	struct GUI_RadioButton* nextRadioButton; // circular one way list
} GUI_RadioButton;
GUI_RadioButton* GUI_radioButton_create(char* defaultText, uint32_t fontID, GUI_CallbackTypedef onClick);
void GUI_radioButton_render(GUI_RadioButton* radioButton, int32_t x, int32_t y);
void GUI_radioButton_click(GUI_RadioButton* radioButton);
GUI_RadioButton* GUI_radioButton_get_selected(GUI_RadioButton* radioButton);

#define GUI_MENU_COLUMN_MAX_ROWS	5
struct GUI_MenuColumn;
typedef struct GUI_MenuColumn {
	struct GUI_MenuColumn* nextCol;
	GUI_Component* rows[GUI_MENU_COLUMN_MAX_ROWS];
	uint32_t numRows;
} GUI_MenuColumn;
GUI_MenuColumn* GUI_menuColumn_create();
int32_t GUI_menuColumn_add_component(GUI_MenuColumn* menuColumn, GUI_Component* component);
void GUI_menuColumn_render(GUI_MenuColumn* menuColumn, int32_t x, int32_t y);
GUI_MenuColumn* GUI_menuColumn_get_previous_col(GUI_MenuColumn* menuColumn);

typedef struct GUI_Menu {
	GUI_MenuColumn* columns;
} GUI_Menu;
GUI_Menu* GUI_menu_create();
void GUI_menu_add_column(GUI_Menu* menu, GUI_MenuColumn* col);
void GUI_menu_render(GUI_Menu* menu, int32_t x, int32_t y);
uint32_t GUI_menu_get_num_columns(GUI_Menu* menu);

typedef struct GUI_MenuButton {
	GUI_ToggleButton* toggleButton;
	
	GUI_Menu* menu;
} GUI_MenuButton;
GUI_MenuButton* GUI_menuButton_create(char* defaultText, uint32_t fontID, GUI_CallbackTypedef onClick, GUI_Menu* menu);
void GUI_menuButton_render(GUI_MenuButton* menuButton, int32_t x, int32_t y);
void GUI_menuButton_click(GUI_MenuButton* menuButton);

typedef enum {
	GUI_GRAPH_MODE_1CH_NORMAL,	// display channel 1
	GUI_GRAPH_MODE_2CH_NORMAL,	// display channel 2
	GUI_GRAPH_MODE_12CH_NORMAL,	// display both channels
	GUI_GRAPH_MODE_XY,			// channel 1 is X, channel 2 is Y
	GUI_GRAPH_MODE_1CH_FFT,		// FFT on channel 1
	GUI_GRAPH_MODE_2CH_FFT,		// FFT on channel 2
	GUI_GRAPH_MODE_AMPLITUDE,	// log-log scale
	GUI_GRAPH_MODE_PHASE		// log scale X, degrees scale Y
} GUI_GraphModeTypedef;

struct GUI_GraphLabel;
struct GUI_Graph;
typedef struct GUI_GraphLabel {
	GUI_ScrollButton* scrollButton;
	struct GUI_Graph* ownerGraph;
	
	GUI_Component* nextLabel;
	MonoGFX_LineStyleTypedef lineStyle;
	uint8_t rightChannel; // bool (true if label should be on right side, only used when vertical=1)
	uint8_t vertical; // bool (true if label is on Y axis)
} GUI_GraphLabel;
GUI_GraphLabel* GUI_graphLabel_create(char* text, MonoGFX_LineStyleTypedef lineStyle, uint8_t floatRight, uint8_t vertical, uint32_t fontID, GUI_CallbackTypedef onScroll);
void GUI_graphLabel_render(GUI_GraphLabel* graphLabel, int32_t x, int32_t y);
void GUI_graphLabel_select_render(GUI_GraphLabel* graphLabel, int32_t x, int32_t y);

typedef struct GUI_Graph {
	uint32_t w;
	uint32_t h;
	
	GUI_Component* vDiv1ScrollButtonComponent; // volts per div
	GUI_Component* vDiv2ScrollButtonComponent; // volts per div
	GUI_Component* hDivScrollButtonComponent;
	
	GUI_GraphLabel* vOffset1GraphLabel;
	GUI_GraphLabel* vOffset2GraphLabel;
	GUI_GraphLabel* hOffsetGraphLabel;
	
	arm_rfft_fast_instance_f32 FFTStruct;
	GUI_GraphModeTypedef mode;
	float Fs_Hz; // sampling frequency
	float* data_1; // [V] buffer center is expected to be at dataBufferSize/2
	float* data_2;
	uint32_t dataBufferSize;
	
	GUI_Component* labels;
} GUI_Graph;
GUI_Graph* GUI_graph_create(uint32_t w, uint32_t h);
void GUI_graph_render(GUI_Graph* graph, int32_t x, int32_t y);
void GUI_graph_add_label(GUI_Graph* graph, GUI_Component* graphLabelComponent);

#define GUI_SCREEN_MAX_COMPONENTS	16
typedef struct GUI_Screen {
	uint32_t id;
	
	GUI_Component* components[GUI_SCREEN_MAX_COMPONENTS];
	GUI_Component* defaultTabComponent;
	uint32_t numComponents;
} GUI_Screen;
GUI_Screen* GUI_screen_create();
int32_t GUI_screen_add_component(GUI_Screen* screen, GUI_Component* component);
void GUI_screen_render(GUI_Screen* screen, int32_t x, int32_t y);

struct GUI_Sprite;
typedef void (*GUI_SpriteRenderCallbackTypedef)(struct GUI_Sprite* sprite, int32_t x, int32_t y);
typedef struct GUI_Sprite {
	GUI_SpriteRenderCallbackTypedef renderCallback;
} GUI_Sprite;
GUI_Sprite* GUI_sprite_create(GUI_SpriteRenderCallbackTypedef renderCallback);
void GUI_sprite_render(GUI_Sprite* sprite, int32_t x, int32_t y);









#endif
