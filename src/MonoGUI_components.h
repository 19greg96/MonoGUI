
#ifndef __MonoGUI_COMPONENTS_H
#define __MonoGUI_COMPONENTS_H

#ifdef MONO_GUI_STM32
	#include "stm32f4xx_hal.h"
	#include "arm_math.h"
	
	#define getTick		HAL_GetTick
#else // arduino
	#include <Arduino.h>
	#define getTick		millis
#endif

#include "MonoGUI_font.h"
#include "MonoGFX.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MonoGUI_CallbackTypedef)(void* caller);
typedef void (*MonoGUI_StringFormatterTypedef)(char* out, void* param);

void formatPercentage(char* out, void* param);
void formatVoltage(char* out, void* param);
void formatFrequency(char* out, void* param);
void formatTime(char* out, void* param);
void formatSimpleText(char* out, void* param);
void formatEmpty(char* out, void* param);

typedef enum {
	MonoGUI_COMPONENT_LABEL				= 0x00U,
	MonoGUI_COMPONENT_BUTTON			= 0x01U,
	MonoGUI_COMPONENT_SCROLL_BUTTON		= 0x02U,
	MonoGUI_COMPONENT_RANGE				= 0x03U,
	MonoGUI_COMPONENT_TOGGLE_BUTTON		= 0x04U,
	MonoGUI_COMPONENT_RADIO_BUTTON		= 0x05U,
	MonoGUI_COMPONENT_MENU_COLUMN		= 0x06U,
	MonoGUI_COMPONENT_MENU				= 0x07U,
	MonoGUI_COMPONENT_MENU_BUTTON		= 0x08U,
	MonoGUI_COMPONENT_GRAPH				= 0x09U,
	MonoGUI_COMPONENT_SCREEN			= 0x0AU,
	MonoGUI_COMPONENT_SPRITE			= 0x0BU,
	MonoGUI_COMPONENT_GRAPH_LABEL		= 0x0CU,
} MonoGUI_ComponentTypedef;

typedef enum {
	MonoGUI_TEXT_ALIGN_LEFT,
	MonoGUI_TEXT_ALIGN_CENTER,
	MonoGUI_TEXT_ALIGN_RIGHT,
	MonoGUI_TEXT_ALIGN_TOP,
	MonoGUI_TEXT_ALIGN_BOTTOM
} MonoGUI_TextAlignTypedef;

typedef enum {
	MonoGUI_TEXT_DIRECTION_HORIZONTAL,
	MonoGUI_TEXT_DIRECTION_VERTICAL
} MonoGUI_TextDirectionTypedef;

struct MonoGUI_Component;
typedef struct MonoGUI_Component {
	uint32_t id;
	int32_t x;
	int32_t y;
	uint32_t width;
	uint32_t height;
	
	uint8_t visible;
	struct MonoGUI_Component* tabNext;
	struct MonoGUI_Component* tabPrev; // circular two way list
	uint8_t selected; // 1 when selected (tab)
	
	MonoGUI_ComponentTypedef type;
	void* component;
} MonoGUI_Component;
MonoGUI_Component* MonoGUI_component_create(MonoGUI_ComponentTypedef type, int32_t x, int32_t y, void* component);
void MonoGUI_component_render(MonoGUI_Component* component);
void MonoGUI_component_set_tabNext(MonoGUI_Component* component, MonoGUI_Component* tabNext);

#define MonoGUI_LABEL_MAX_LEN	16
typedef struct MonoGUI_Label {
	char value[MonoGUI_LABEL_MAX_LEN];
	MonoGUI_StringFormatterTypedef formatter;
	MonoGUI_TextAlignTypedef align;
	uint32_t font;
} MonoGUI_Label;
MonoGUI_Label* MonoGUI_label_create(char* text, uint32_t fontID);
void MonoGUI_label_render(MonoGUI_Label* label, int32_t x, int32_t y);
void MonoGUI_label_getText(MonoGUI_Label* label, char* out);

typedef struct MonoGUI_Button {
	MonoGUI_CallbackTypedef onClick;
	MonoGUI_Label* label;
} MonoGUI_Button;
MonoGUI_Button* MonoGUI_button_create(char* text, uint32_t fontID, MonoGUI_CallbackTypedef onClick);
void MonoGUI_button_render(MonoGUI_Button* button, int32_t x, int32_t y);

#define MonoGUI_SCROLL_BUTTON_PANEL_SHOW_TIMEOUT	1500U	// time for panel to hide after scroll event in ms
typedef struct MonoGUI_ScrollButton {
	MonoGUI_Button* button;
	
	float min;
	float max;
	float smallStep;
	float largeStep;
	float value;
	uint8_t labelIsValue;
	uint8_t panelEnabled;
	uint32_t lastScrollTime;
	MonoGUI_CallbackTypedef onScroll;
} MonoGUI_ScrollButton;
MonoGUI_ScrollButton* MonoGUI_scrollButton_create(char* text, uint32_t fontID, float min, float max, float val, MonoGUI_CallbackTypedef onScroll);
void MonoGUI_scrollButton_render(MonoGUI_ScrollButton* scrollButton, int32_t x, int32_t y);
void MonoGUI_scrollButton_clamp_value(MonoGUI_ScrollButton* scrollButton);
void MonoGUI_scrollButton_scroll(MonoGUI_ScrollButton* scrollButton, int16_t delta, uint8_t largeStep);

typedef struct MonoGUI_Range {
	MonoGUI_ScrollButton* scrollButton;
	MonoGUI_Label* valueLabel;
	MonoGUI_TextAlignTypedef textAlign;
	
	uint32_t size; // width or height in pixels
	uint8_t vertical; // 1 for vertical, 0 for horizontal
	// uint8_t labelAlign; // left or right
} MonoGUI_Range;
MonoGUI_Range* MonoGUI_range_create(char* text, uint32_t fontID, float min, float max, float val, MonoGUI_CallbackTypedef onScroll, uint32_t size, MonoGUI_StringFormatterTypedef valueFromatter);
void MonoGUI_range_render(MonoGUI_Range* range, int32_t x, int32_t y);
void MonoGUI_range_select_render(MonoGUI_Range* range, int32_t x, int32_t y);

typedef struct MonoGUI_ToggleButton {
	MonoGUI_Button* button;
	
	char defaultText[MonoGUI_LABEL_MAX_LEN]; // label text when not checked
	char checkedText[MonoGUI_LABEL_MAX_LEN]; // label text when checked
	uint8_t checked;
	uint8_t showCheckbox; // 1 to show box on right
} MonoGUI_ToggleButton;
MonoGUI_ToggleButton* MonoGUI_toggleButton_create(char* defaultText, uint32_t fontID, MonoGUI_CallbackTypedef onClick);
void MonoGUI_toggleButton_render(MonoGUI_ToggleButton* toggleButton, int32_t x, int32_t y);
void MonoGUI_toggleButton_click(MonoGUI_ToggleButton* toggleButton);

struct MonoGUI_RadioButton;
typedef struct MonoGUI_RadioButton {
	MonoGUI_ToggleButton* toggleButton;
	
	struct MonoGUI_RadioButton* nextRadioButton; // circular one way list
} MonoGUI_RadioButton;
MonoGUI_RadioButton* MonoGUI_radioButton_create(char* defaultText, uint32_t fontID, MonoGUI_CallbackTypedef onClick);
void MonoGUI_radioButton_render(MonoGUI_RadioButton* radioButton, int32_t x, int32_t y);
void MonoGUI_radioButton_click(MonoGUI_RadioButton* radioButton);
MonoGUI_RadioButton* MonoGUI_radioButton_get_selected(MonoGUI_RadioButton* radioButton);

#define MonoGUI_MENU_COLUMN_MAX_ROWS	5
struct MonoGUI_MenuColumn;
typedef struct MonoGUI_MenuColumn {
	struct MonoGUI_MenuColumn* nextCol;
	MonoGUI_Component* rows[MonoGUI_MENU_COLUMN_MAX_ROWS];
	uint32_t numRows;
} MonoGUI_MenuColumn;
MonoGUI_MenuColumn* MonoGUI_menuColumn_create();
int32_t MonoGUI_menuColumn_add_component(MonoGUI_MenuColumn* menuColumn, MonoGUI_Component* component);
void MonoGUI_menuColumn_render(MonoGUI_MenuColumn* menuColumn, int32_t x, int32_t y);
MonoGUI_MenuColumn* MonoGUI_menuColumn_get_previous_col(MonoGUI_MenuColumn* menuColumn);

typedef struct MonoGUI_Menu {
	MonoGUI_MenuColumn* columns;
} MonoGUI_Menu;
MonoGUI_Menu* MonoGUI_menu_create();
void MonoGUI_menu_add_column(MonoGUI_Menu* menu, MonoGUI_MenuColumn* col);
void MonoGUI_menu_render(MonoGUI_Menu* menu, int32_t x, int32_t y);
uint32_t MonoGUI_menu_get_num_columns(MonoGUI_Menu* menu);

typedef struct MonoGUI_MenuButton {
	MonoGUI_ToggleButton* toggleButton;
	
	MonoGUI_Menu* menu;
} MonoGUI_MenuButton;
MonoGUI_MenuButton* MonoGUI_menuButton_create(char* defaultText, uint32_t fontID, MonoGUI_CallbackTypedef onClick, MonoGUI_Menu* menu);
void MonoGUI_menuButton_render(MonoGUI_MenuButton* menuButton, int32_t x, int32_t y);
void MonoGUI_menuButton_click(MonoGUI_MenuButton* menuButton);

typedef enum {
	MonoGUI_GRAPH_MODE_1CH_NORMAL,	// display channel 1
	MonoGUI_GRAPH_MODE_2CH_NORMAL,	// display channel 2
	MonoGUI_GRAPH_MODE_12CH_NORMAL,	// display both channels
	MonoGUI_GRAPH_MODE_XY,			// channel 1 is X, channel 2 is Y
	MonoGUI_GRAPH_MODE_1CH_FFT,		// FFT on channel 1
	MonoGUI_GRAPH_MODE_2CH_FFT,		// FFT on channel 2
	MonoGUI_GRAPH_MODE_AMPLITUDE,	// log-log scale
	MonoGUI_GRAPH_MODE_PHASE		// log scale X, degrees scale Y
} MonoGUI_GraphModeTypedef;

struct MonoGUI_GraphLabel;
struct MonoGUI_Graph;
typedef struct MonoGUI_GraphLabel {
	MonoGUI_ScrollButton* scrollButton;
	struct MonoGUI_Graph* ownerGraph;
	
	MonoGUI_Component* nextLabel;
	MonoGFX_LineStyleTypedef lineStyle;
	uint8_t rightChannel; // bool (true if label should be on right side, only used when vertical=1)
	uint8_t vertical; // bool (true if label is on Y axis)
} MonoGUI_GraphLabel;
MonoGUI_GraphLabel* MonoGUI_graphLabel_create(char* text, MonoGFX_LineStyleTypedef lineStyle, uint8_t floatRight, uint8_t vertical, uint32_t fontID, MonoGUI_CallbackTypedef onScroll);
void MonoGUI_graphLabel_render(MonoGUI_GraphLabel* graphLabel, int32_t x, int32_t y);
void MonoGUI_graphLabel_select_render(MonoGUI_GraphLabel* graphLabel, int32_t x, int32_t y);

typedef struct MonoGUI_Graph {
	uint32_t w;
	uint32_t h;
	
	MonoGUI_Component* vDiv1ScrollButtonComponent; // volts per div
	MonoGUI_Component* vDiv2ScrollButtonComponent; // volts per div
	MonoGUI_Component* hDivScrollButtonComponent;
	
	MonoGUI_GraphLabel* vOffset1GraphLabel;
	MonoGUI_GraphLabel* vOffset2GraphLabel;
	MonoGUI_GraphLabel* hOffsetGraphLabel;
	
#ifdef MONO_GUI_STM32
	arm_rfft_fast_instance_f32 FFTStruct;
#endif
	MonoGUI_GraphModeTypedef mode;
	float Fs_Hz; // sampling frequency
	float* data_1; // [V] buffer center is expected to be at dataBufferSize/2
	float* data_2;
	uint32_t dataBufferSize;
	
	MonoGUI_Component* labels;
} MonoGUI_Graph;
MonoGUI_Graph* MonoGUI_graph_create(uint32_t w, uint32_t h);
void MonoGUI_graph_render(MonoGUI_Graph* graph, int32_t x, int32_t y);
void MonoGUI_graph_add_label(MonoGUI_Graph* graph, MonoGUI_Component* graphLabelComponent);

#define MonoGUI_SCREEN_MAX_COMPONENTS	16
typedef struct MonoGUI_Screen {
	uint32_t id;
	
	MonoGUI_Component* components[MonoGUI_SCREEN_MAX_COMPONENTS];
	MonoGUI_Component* defaultTabComponent;
	uint32_t numComponents;
} MonoGUI_Screen;
MonoGUI_Screen* MonoGUI_screen_create();
int32_t MonoGUI_screen_add_component(MonoGUI_Screen* screen, MonoGUI_Component* component);
void MonoGUI_screen_render(MonoGUI_Screen* screen, int32_t x, int32_t y);

struct MonoGUI_Sprite;
typedef void (*MonoGUI_SpriteRenderCallbackTypedef)(struct MonoGUI_Sprite* sprite, int32_t x, int32_t y);
typedef struct MonoGUI_Sprite {
	MonoGUI_SpriteRenderCallbackTypedef renderCallback;
} MonoGUI_Sprite;
MonoGUI_Sprite* MonoGUI_sprite_create(MonoGUI_SpriteRenderCallbackTypedef renderCallback);
void MonoGUI_sprite_render(MonoGUI_Sprite* sprite, int32_t x, int32_t y);



#ifdef __cplusplus
}
#endif





#endif
