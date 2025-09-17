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
#define tab_space 4 // number of spaces for a tab

// For accessing the font library
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))




void tft_write(unsigned char c) ;
//static void setTextCursor(short x, short y);
//static void setTextColor(char c);
//static void setTextColor(char c, char bg);
//static void setTextSize(unsigned char s);

static void printString(char* str) ;
static void pchar(char c);


void clrscr();

// 5x7 font
static void writeStringBold(char* str);

void drawChar(unsigned char c, color_t color, color_t bg, unsigned char size) ;


//NOVAS IMPLEMENTAÇÕES
enum screenMode {
    MODE_320x240,
    MODE_640x480
};
typedef enum screenMode screenMode_t ;
typedef struct vga16_text vga16_text_t ;

struct vga16_text {
  //void (*print_string)(vga16_text_t* self, char* str);
  void (*printString)(char* str);
  void (*setTextColor)(char c, char b);
  void (*setTextSize)(unsigned char s);
  void (*setTextCursor)(short x, short y);
  void (*clrscr)(void);
  void * _private;
};

vga16_text_t* create_screen(screenMode_t mode);
void put_cursor(unsigned char c);



#endif