#ifndef __VGA16_TEXT_H__
#define __VGA16_TEXT_H__    

/**
 * Hunter Adams (vha3@cornell.edu)
 * modifed for text by pdsilva AKA (pgordão)
 *
 */
#include "vga16_drv.h"
#include "cursor.h"
#include "colors.h"
#include "font.h"

// Bit masks for drawPixel routine
#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

// For drawLine
#define swap(a, b) { short t = a; a = b; b = t; }

// For writing text
#define tabspace 4 // number of spaces for a tab

// For accessing the font library
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

typedef struct {
    uint8_t name[32];
    uint16_t width;
    uint16_t height;
    uint8_t tab_space; // font size multiplier
    color_t textcolor;
    color_t textbgcolor;
    font_t font; 
    cursor_t cursor;  
} screenMode_t;




void tft_write(unsigned char c) ;
void setTextCursor(short x, short y);
void setTextColor(char c);
void setTextColor2(char c, char bg);
void setTextSize(unsigned char s);
void setTextWrap(char w);
void printString(char* str) ;
void pchar(char c);
unsigned short get_cursor_y();
unsigned short get_textsize();
unsigned short get_cursor_x();
void init_text_screen(char mode);
void clrscr();

// 5x7 font
void writeStringBold(char* str);

void drawChar(unsigned char c, color_t color, color_t bg, unsigned char size) ;


#endif