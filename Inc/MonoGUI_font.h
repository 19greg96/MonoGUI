#ifndef __GUI_FONT__
#define __GUI_FONT__

typedef struct GUI_FontCharInfoTypedef {
    const unsigned char widthBits; // width, in bits (or pixels), of the character
    const unsigned int offset;
} GUI_FontCharInfoTypedef;

typedef struct GUI_FontInfoTypedef {
   const unsigned int glyph_height;			/* glyph height in pixels				*/
   const unsigned int start_char;			/* start char							*/
   const unsigned int end_char;				/* end char								*/
   const GUI_FontCharInfoTypedef *mapping_table;		/* used to find the index of a glyph	*/
   const unsigned char *glyph_table;		/* font table start address in memory	*/
} GUI_FontInfoTypedef;


         
#endif
                                  

