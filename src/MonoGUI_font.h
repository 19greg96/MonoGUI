#ifndef __MonoGUI_FONT__
#define __MonoGUI_FONT__

#include "MonoGUI_config.h"
#include <stdint.h>

#ifdef MONO_GUI_STM32
	#define PROGMEM
	#define MONO_GUI_READ_BYTE_FROM_PROGMEM(x)		(x)
	#define MONO_GUI_GET_CHARACTER_WIDTH(f, chr)	(f->mapping_table[chr].widthBits)
	#define MONO_GUI_GET_CHARACTER_OFFSET(f, chr)	(f->mapping_table[chr].offset)
#else // arduino
	#include <avr/pgmspace.h>
	#define MONO_GUI_READ_BYTE_FROM_PROGMEM(x)	pgm_read_byte(&(x))
	#define MONO_GUI_GET_CHARACTER_WIDTH(f, chr)	MONO_GUI_READ_BYTE_FROM_PROGMEM(f->mapping_table[chr].widthBits)
	#define MONO_GUI_GET_CHARACTER_OFFSET(f, chr)	pgm_read_word(&(f->mapping_table[chr].offset))
#endif

typedef struct MonoGUI_FontCharInfoTypedef {
    const uint8_t widthBits; // width, in bits (or pixels), of the character
    const uint16_t offset;
} MonoGUI_FontCharInfoTypedef;

typedef struct MonoGUI_FontInfoTypedef {
   const uint8_t glyph_height;								/* glyph height in pixels				*/
   const uint8_t start_char;								/* start char							*/
   const uint8_t end_char;									/* end char								*/
   const MonoGUI_FontCharInfoTypedef *mapping_table;		/* used to find the index of a glyph	*/
   const unsigned char *glyph_table;						/* font table start address in memory	*/
} MonoGUI_FontInfoTypedef;


#endif
                                  

