#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
// Our assembled programs:

// Header file
#include "vga16_text.h"
// Font file
#include "glcdfont.c"
//#include "font_rom_brl4.h"
#include <string.h>

// 5x7 font
void writeStringBold(char* str);

// For drawing characters
unsigned short cursor_y, cursor_x, textsize ;
color_t textcolor, textbgcolor;
char wrap;


// Screen width/height
int _width=0;  //640
int _height=0; //480



void init_text_screen(char mode){
  
  if(mode==0){    
    cursor_x = 0 ;
    cursor_y = 0 ;
    textsize = 1 ;
    textcolor = WHITE ;
    textbgcolor = BLACK ;
    wrap = true ;
    _width = 320;
    _height= 240;
    //cursor_init(cursor) ;
  }
  else if(mode==1){    
    cursor_x = 0 ;
    cursor_y = 0 ;
    textsize = 1 ;
    textcolor = WHITE ;
    textbgcolor = BLACK ;
    wrap = true ;
    _width  = 640;
    _height = 480;
    //cursor_init(cursor) ;
  }

}

void clrscr(){
  // clear the screen
  memset(&vga_data_array[0], 0, TXCOUNT) ;
  // reset cursor position
  cursor_x = 0 ;
  cursor_y = 0 ;
}

inline void pchar(char c){
    tft_write(c);
}

// A function for drawing a pixel with a specified color.
// Note that because information is passed to the PIO state machines through
// a DMA channel, we only need to modify the contents of the array and the
// pixels will be automatically updated on the screen.
void drawPixel(short x, short y, color_t color) {
    // Range checks (640x480 display)
    // if (x > 639) x = 639 ;
    // if (x < 0) x = 0 ;
    // if (y < 0) y = 0 ;
    // if (y > 479) y = 479 ;
    if((x > (_width-1)) | (x < 0) | (y > (_height-1)) | (y < 0) ) return;

    // Which pixel is it?
    int pixel = ((_width * y) + x) ;

    // Is this pixel stored in the first 4 bits
    // of the vga data array index, or the second
    // 4 bits? Check, then mask.
    if (pixel & 1) {
        vga_data_array[pixel>>1] = (vga_data_array[pixel>>1] & TOPMASK) | (color << 4) ;
    }
    else {
        vga_data_array[pixel>>1] = (vga_data_array[pixel>>1] & BOTTOMMASK) | (color) ;
    }
}

void drawHLine(int x, int y, int w, color_t color) {
  // range checks
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x - 1;
  //
  //short xx = x;
  short both_color = color | (color<<4) ;
  // loner pixel at x -- align left with next byte boundary
  if((x & 1)) {
    drawPixel(x,y,color);
    x++ ;
    w-- ;
  }
  // draw loner pixel at end and adjust width
  if((w & 1)){
    drawPixel(x+w-1, y, color);
    w-- ;
  }
  // draw rest of line
  int len = (w>>1)  ;
  if (len>0 && y<480 ) memset(&vga_data_array[320*y+(x>>1)], both_color, len) ;
 // original code
    // for (int i=x; i<=(x+w); i++) {
    //     drawPixel(i, y, color) ;
    // }
}



// fill a rectangle
void fillRect(short x, short y, short w, short h, color_t color) {
/* Draw a filled rectangle with starting top-left vertex (x,y),
 *  width w and height h with given color
 * Parameters:
 *      x:  x-coordinate of top-left vertex; top left of screen is x=0
 *              and x increases to the right
 *      y:  y-coordinate of top-left vertex; top left of screen is y=0
 *              and y increases to the bottom
 *      w:  width of rectangle
 *      h:  height of rectangle
 *      color:  3-bit color value
 * Returns:     Nothing
 */

  // rudimentary clipping (drawChar w/big text requires this)
  // if((x >= _width) || (y >= _height)) return;
  // if((x + w - 1) >= _width)  w = _width  - x - 1;
   if((y + h - 1) >= _height) h = _height - y - 1;

  // tft_setAddrWindow(x, y, x+w-1, y+h-1);

  // for(int i=x; i<(x+w); i++) {
  //   for(int j=y; j<(y+h); j++) {
  //       drawPixel(i, j, color);
  //   }
  // }
  for(int j=y; j<(y+h); j++) {
    drawHLine(x, j, w, color) ;
  }
}


// Draw a character
static void drawChar_interna(short x, short y, unsigned char c, color_t color, color_t bg, unsigned char size) {
    char i, j;
  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

    // Calcula a área do caractere
    short charWidth = 6 * size;
    short charHeight = 8 * size;    
    // PRIMEIRO: Limpa a área completa do caractere
    fillRect(x, y, charWidth, charHeight, BLACK);

  for (i=0; i<6; i++ ) {
    unsigned char line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font+(c*5)+i);
    for ( j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void drawChar(unsigned char c, color_t color, color_t bg, unsigned char size) {
  drawChar_interna( cursor_x, cursor_y, c, color, bg, size);
}

unsigned short get_cursor_y(){
  return cursor_y;
}
unsigned short get_cursor_x()
{
  return cursor_x;
}
unsigned short get_textsize()
{
  return textsize;
} 

inline void setTextCursor(short x, short y) {
/* Set cursor for text to be printed
 * Parameters:
 *      x = x-coordinate of top-left of text starting
 *      y = y-coordinate of top-left of text starting
 * Returns: Nothing
 */
  cursor_x = x;
  cursor_y = y;
}

inline void setTextSize(unsigned char s) {
/*Set size of text to be displayed
 * Parameters:
 *      s = text size (1 being smallest)
 * Returns: nothing
 */
  textsize = (s > 0) ? s : 1;
}

inline void setTextColor(char c) {
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

inline void setTextColor2(char c, char b) {
/* Set color of text to be displayed
 * Parameters:
 *      c = 16-bit color of text
 *      b = 16-bit color of text background
 */
  textcolor   = c;
  textbgcolor = b;
}

inline void setTextWrap(char w) {
  wrap = w;
}

void tft_write(unsigned char c){
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else if (c == '\t'){
      int new_x = cursor_x + tabspace;
      if (new_x < _width){
          cursor_x = new_x;
      }
  } else {
    drawChar_interna(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (/*wrap &&*/ (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
}

inline void printString(char* str){
/* Print text onto screen
 * Call tft_setCursor(), tft_setTextColor(), tft_setTextSize()
 *  as necessary before printing
 */
    while (*str){
        tft_write(*str++);
    }
}

//=================================================
// added 10/16/2023 brl4
inline void setTextColorBig(color_t color, char background) {
/* Set color of text to be displayed
 * Parameters:
 *      color = 16-bit color of text
 *      b = 16-bit color of text background
 *      background ==-1 means trasnparten background
 */
  textcolor   = color;
  textbgcolor = background;
}
 
inline void writeStringBold(char* str){
/* Print text onto screen
 * Call tft_setCursor(), tft_setTextColorBig()
 *  as necessary before printing
 */
   /* Print text onto screen
 * Call tft_setCursor(), tft_setTextColor(), tft_setTextSize()
 *  as necessary before printing
 */
    char temp_bg ;
    temp_bg = textbgcolor;
    while (*str){
        char c = *str++;
        drawChar_interna(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        drawChar_interna(cursor_x+1, cursor_y, c, textcolor, textcolor, textsize);
        cursor_x += 7 * textsize ;
    }
    textbgcolor = temp_bg ;
}


//////////////////////////////////////////////////
// get the color of a pixel
short readPixel(short x, short y) {
  // Which pixel is it?
  int pixel = ((640 * y) + x) ;
  short color ;
  // Is this pixel stored in the first 4 bits
  // of the vga data array index, or the second
  // 4 bits? Check, then mask.
  if (pixel & 1) {
      color = vga_data_array[pixel>>1] >> 4 ;
  }
  else {
      color = vga_data_array[pixel>>1] & 0xf  ;
  }
  return color ;
}

 
 