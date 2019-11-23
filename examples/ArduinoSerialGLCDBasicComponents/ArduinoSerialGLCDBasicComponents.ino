#include <stdint.h>
#include <MonoGFX.h>
#include <MonoGUI.h>
#include <MonoGUI_font_legible3x5_6pt.h>
#include <Drivers/Arduino/KS0107Serial.h>

/*

You need:
 - An Arduino
 - Two SNx4HC595 or equivalent shift registers
 - Monochromatic LCD display with KS0107/S6B0107/SBN6400 compatible driver
 - 10k Potentiometer

Connections on Arduino Nano:
 - D8 - pin 12 (RCLK) of both shift registers
 - D11 - pin 14 (SER) of first shift register
 - D13 - pin 11 (SRCLK) of both shift registers
 - RST - RST on display module
Connections on shift registers:
 - First shift register
  - 15 (Qa) - DB0 on display
  - 1 (Qb) - DB1 on display
  - 2 (Qc) - DB2 on display
  - 3 (Qd) - DB3 on display
  - 4 (Qe) - DB4 on display
  - 5 (Qf) - DB5 on display
  - 6 (Qg) - DB6 on display
  - 7 (Qh) - DB7 on display
 - Second shift register
  - 1 (Qb) - /CS1 on display
  - 2 (Qc) - /CS2 on display
  - 3 (Qd) - D/I on display
  - 4 (Qe) - E on display
 - Chain shift registers:
  - 9 (Qh' first shift register) - 14 (SER second shift register)
  - Don't forget to connect Vcc and GND of shift registers
Connections on display:
 - Vdd - +5V
 - Vss - Ground
 - R/W - Ground
 - V0 (for contrast adjustment) - Pot center
 - -5V - Pot side. Other side of pot is Ground
 - LED+ - +5V
 - LED- - Ground
*/


MonoGFX_DisplayTypedef MonoGFXDisplay;

KS0107Serial serialDisplay(8); // SPI_latchPin

void onBtn(void* caller) {
	// (MonoGUI_Button*)caller
}

void setup() {
	MonoGUI_init();
	uint32_t defaultFont = MonoGUI_register_font(&MonoGUI_font_legible3x5_6ptFontInfo);
	
	MonoGUI_Screen* mainScreen = MonoGUI_screen_create();
	
	MonoGUI_Component* btn = MonoGUI_component_create(MonoGUI_COMPONENT_BUTTON, 5, 5, MonoGUI_button_create("Button", defaultFont, onBtn));
	
	MonoGUI_screen_add_component(mainScreen, btn);
	
	MonoGFXDisplay.width = 128;
	MonoGFXDisplay.height = 64;
	MonoGFXDisplay.mode = MonoGFX_DISPLAY_MODE_VERTICAL;
	MonoGFX_init(&MonoGFXDisplay);
}

void loop() {
	MonoGUI_render();
	serialDisplay.update(MonoGFXDisplay.buffer);
}

