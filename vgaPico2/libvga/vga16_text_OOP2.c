#include "vga16_text.h"
#include "glcdfont.c"
#include <string.h>
#include <stdlib.h>

// Estrutura privada
typedef struct {
    uint16_t cursor_y;
    uint16_t cursor_x;
    uint8_t textsize;
    color_t textcolor;
    color_t textbgcolor;
    bool wrap;
    uint16_t width;
    uint16_t height;

    uint8_t* vga_data_array;
    uint32_t txcount;
    uint16_t TOPMASK;
    uint16_t BOTTOMMASK;
    uint8_t tabspace;
    
} vga16_text_private_t;

void vga16_text_init(vga16_text_t* vga, uint8_t mode) ;
void vga16_text_clrscr(vga16_text_t* vga) ;
void vga16_text_pchar(vga16_text_t* vga, char c) ;
void vga16_text_draw_pixel(vga16_text_t* vga, int16_t x, int16_t y, color_t color) ;
void vga16_text_draw_hline(vga16_text_t* vga, int16_t x, int16_t y, int16_t w, color_t color) ;
void vga16_text_fill_rect(vga16_text_t* vga, int16_t x, int16_t y, int16_t w, int16_t h, color_t color) ;
void vga16_text_draw_char_interna(vga16_text_t* vga, uint8_t c) ;
void vga16_text_draw_char(vga16_text_t* vga, uint8_t c) ;
void vga16_text_set_text_cursor(vga16_text_t* vga, int16_t x, int16_t y) ;
void vga16_text_set_text_color(vga16_text_t* vga, color_t c) ;
void vga16_text_set_text_bg_color(vga16_text_t* vga, color_t c) ;
void vga16_text_set_text_color2(vga16_text_t* vga, color_t c, color_t b) ;
void vga16_text_set_text_wrap(vga16_text_t* vga, bool w) ;
void vga16_text_tft_write(vga16_text_t* vga, uint8_t c) ;
void vga16_text_print_string(vga16_text_t* vga, char* str) ;
void vga16_text_set_text_color_big(vga16_text_t* vga, color_t color, color_t background) ;
void vga16_text_write_string_bold(vga16_text_t* vga, int8_t* str) ;
color_t vga16_text_read_pixel(const vga16_text_t* vga, int16_t x, int16_t y) ;

uint8_t vga16_text_get_size(const vga16_text_t* vga);
void vga16_text_set_size(const vga16_text_t* vga, uint8_t s);
color_t vga16_text_get_color(vga16_text_t* vga);
color_t vga16_text_get_bgcolor(const vga16_text_t* vga);


uint16_t vga16_text_get_cursor_y(const vga16_text_t* vga);
uint16_t vga16_text_get_cursor_x(const vga16_text_t* vga);
uint8_t  vga16_text_get_textsize(const vga16_text_t* vga);
uint16_t vga16_text_get_width(const vga16_text_t* vga);
uint16_t vga16_text_get_height(const vga16_text_t* vga);

void vga16_text_set_cursor(vga16_text_t* vga, int16_t x, int16_t y);
void vga16_text_set_color(vga16_text_t* vga, color_t c);
void vga16_text_set_bgcolor(vga16_text_t* vga, color_t c);
void vga16_text_set_color2(vga16_text_t* vga, color_t c, color_t b);
void vga16_text_set_wrap(vga16_text_t* vga, bool w);



// Construtor
vga16_text_t* vga16_text_create(uint8_t mode) {
    vga16_text_t* vga = (vga16_text_t*)malloc(sizeof(vga16_text_t));
    vga16_text_private_t* priv = (vga16_text_private_t*)malloc(sizeof(vga16_text_private_t));
    
    if (!vga || !priv) {
        free(vga);
        free(priv);
        return NULL;
    }
    
    vga->_private = priv;

    // Inicialização básica
    if(mode==0){    
        priv->cursor_x=0 ;
        priv->cursor_y=0 ;
        priv->textsize=1 ;
        priv->textcolor=WHITE ;
        priv->textbgcolor=BLACK;
        priv->wrap=true;
        priv->width=320;
        priv->height=240;
    }
    else if(mode==1){    
        priv->cursor_x=0 ;
        priv->cursor_y=0 ;
        priv->textsize=1 ;
        priv->textcolor=WHITE ;
        priv->textbgcolor=BLACK;
        priv->wrap=true;
        priv->width=640;
        priv->height=480;
    } else {
        priv->cursor_x=0 ;
        priv->cursor_y=0 ;
        priv->textsize=1 ;
        priv->textcolor=WHITE ;
        priv->textbgcolor=BLACK;
        priv->wrap=true;
        priv->width=640;
        priv->height=480;
    }

    vga->init = vga16_text_init;
    vga->clrscr = vga16_text_clrscr;
    vga->pchar = vga16_text_pchar;
    vga->draw_pixel = vga16_text_draw_pixel;
    vga->draw_hline = vga16_text_draw_hline;
    vga->fill_rect = vga16_text_fill_rect;
    vga->draw_char_interna = vga16_text_draw_char_interna;
    vga->draw_char = vga16_text_draw_char;
    vga->print_string = vga16_text_print_string;
    vga->tft_write = vga16_text_tft_write;
   // vga->set_text_color_big = vga16_text_set_text_color_big;
    vga->write_string_bold = vga16_text_write_string_bold;
    vga->read_pixel = vga16_text_read_pixel;
 
    // Getters
    //vga->get_cursor_x = vga16_text_get_cursor_x;
    //vga->get_cursor_y = vga16_text_get_cursor_y;
    //vga->get_width = vga16_text_get_width;
    //vga->get_height = vga16_text_get_height;
    vga->get_text_size = vga16_text_get_size;
    vga->get_text_bgcolor = vga16_text_get_bgcolor;

    // Setters
    //vga->set_cursor_x = vga16_text_set_cursor_x;
    //vga->set_cursor_y = vga16_text_set_cursor_y;
    //vga->set_width = vga16_text_set_width;
    //vga->set_height = vga16_text_set_height;
    vga->set_text_size = vga16_text_set_size;
    //vga->set_text_bg_color = vga16_text_set_bgcolor;
    vga->set_text_cursor = vga16_text_set_cursor;
    vga->set_text_color = vga16_text_set_color;
    //vga->set_text_color2 = vga16_text_set_text_color2;  
    //vga->set_text_wrap = vga16_text_set_text_wrap;


    return vga;
}

// Destrutor
void vga16_text_destroy(vga16_text_t* vga) {
    if (vga) {
        free(vga->_private);
        free(vga);
    }
}

// Implementações dos métodos (apenas esqueleto)
void vga16_text_init(vga16_text_t* vga, uint8_t mode) {
    // Seu código aqui
}

void vga16_text_clrscr(vga16_text_t* vga) {
 vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  // clear the screen
  memset(&priv->vga_data_array[0], 0, priv->txcount) ;
  // reset cursor position
  priv->cursor_x = 0 ;
  priv->cursor_y = 0 ;
}

void vga16_text_pchar(vga16_text_t* vga, char c) {
    vga16_text_tft_write(vga,c);
}

void vga16_text_draw_pixel(vga16_text_t* vga, int16_t x, int16_t y, color_t color) {
    vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
    if((x > (priv->width-1)) | (x < 0) | (y > (priv->height-1)) | (y < 0) )
        return;

    int pixel = ((priv->width * y) + x) ;

    if (pixel & 1) {
        priv->vga_data_array[pixel>>1] = (priv->vga_data_array[pixel>>1] & priv->TOPMASK) | (color << 4) ;
    }
    else {
        priv->vga_data_array[pixel>>1] = (priv->vga_data_array[pixel>>1] & priv->BOTTOMMASK) | (color) ;
    }
}

void vga16_text_draw_hline(vga16_text_t* vga, int16_t x, int16_t y, int16_t w, color_t color) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  if((x >= priv->width) || (y >= priv->height))
      return;
  if((x + w - 1) >= priv->width)
      w = priv->width  - x - 1;
  short both_color = color | (color<<4) ;
  if((x & 1)) {
    vga16_text_draw_pixel(vga,x,y,color);
    x++ ;
    w-- ;
  }
  if((w & 1)){
    vga16_text_draw_pixel(vga,x+w-1, y, color);
    w-- ;
  }
  int len = (w>>1)  ;
  if (len>0 && y<480 )
      memset(&priv->vga_data_array[320*y+(x>>1)], both_color, len) ;
}

void vga16_text_fill_rect(vga16_text_t* vga, int16_t x, int16_t y, int16_t w, int16_t h, color_t color) {
    if((y + h - 1) >= vga->height) h = vga->height - y - 1;

    for(int j=y; j<(y+h); j++) {
      vga16_text_draw_hline(vga,x, j, w, color) ;
    }
}

void vga16_text_draw_char_interna(vga16_text_t* vga, uint8_t c){
    vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
    int16_t x = priv->cursor_x;
    int16_t y = priv->cursor_y;
    color_t color = priv->textcolor;
    color_t bg = priv->textbgcolor;
    uint8_t size = priv->textsize;

    char i, j;
    if((x >= vga->width)            || // Clip right
        (y >= vga->height)           || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0))   // Clip top
        return;

    short charWidth = 6 * size;
    short charHeight = 8 * size;
    // PRIMEIRO: Limpa a área completa do caractere
    vga16_text_fill_rect(vga,x, y, charWidth, charHeight, BLACK);

  for (i=0; i<6; i++ ) {
    unsigned char line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font+(c*5)+i);
    for ( j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          vga16_text_draw_pixel(vga,x+i, y+j, color);
        else {  // big size
          vga16_text_fill_rect(vga,x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
          vga16_text_draw_pixel(vga,x+i, y+j, bg);
        else {  // big size
          vga16_text_fill_rect(vga,x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void vga16_text_draw_char(vga16_text_t* vga, uint8_t c) {
    vga16_text_draw_char_interna( vga, c);
}


void vga16_text_tft_write(vga16_text_t* vga, uint8_t c) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;

  if (c == '\n') {
    priv->cursor_y += priv->textsize*8;
    priv->cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else if (c == '\t'){
      int new_x = priv->cursor_x + priv->tabspace;
      if (new_x < priv->width){
          priv->cursor_x = new_x;
      }
  } else {
    vga16_text_draw_char_interna(vga, c);
    priv->cursor_x += priv->textsize*6;
    if (/*wrap &&*/ (priv->cursor_x > (priv->width - priv->textsize*6))) {
      priv->cursor_y += priv->textsize*8;
      priv->cursor_x = 0;
    }
  }
}

void vga16_text_print_string(vga16_text_t* vga, char* str) {
    while (*str){
        vga16_text_tft_write(vga,*str++);
    }
}

void vga16_text_write_string_bold(vga16_text_t* vga, int8_t* str) {
    vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
    char temp_bg ;
    temp_bg = priv->textbgcolor;
    while (*str){
        char c = *str++;
        vga16_text_draw_char_interna(vga, c);
        vga16_text_draw_char_interna(vga, c);
        uint16_t cursor_x = priv->cursor_x;
        cursor_x += 7 * priv->textsize;
        priv->cursor_x = cursor_x ;
    }
    priv->textbgcolor = temp_bg ;
}

color_t vga16_text_read_pixel(const vga16_text_t* vga, int16_t x, int16_t y) {
  int pixel = ((640 * y) + x) ;
  short color ;
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  // Is this pixel stored in the first 4 bits
  // of the vga data array index, or the second
  // 4 bits? Check, then mask.
  if (pixel & 1) {
      color = priv->vga_data_array[pixel>>1] >> 4 ;
  }
  else {
      color = priv->vga_data_array[pixel>>1] & 0xf  ;
  }
  return color ;
}








    // Getters
uint8_t vga16_text_get_size(const vga16_text_t* vga) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  return priv->textsize;
}
color_t vga16_text_get_bgcolor(const vga16_text_t* vga) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  return priv->textbgcolor;
}

    // Setters
void vga16_text_set_size(const vga16_text_t* vga, uint8_t s) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  priv->textsize = (s > 0) ? s : 1;
}

void vga16_text_set_color(vga16_text_t* vga, color_t c) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  priv->textcolor = c;
}

void vga16_text_set_color2(vga16_text_t* vga, color_t c, color_t b) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  priv->textcolor   = c;
  priv->textbgcolor = b;
}

    void (*set_text_bgcolor)(vga16_text_t* self, color_t c);
void vga16_text_set_bgcolor(vga16_text_t* vga, color_t c) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  priv->textbgcolor = c;
}

void vga16_text_set_cursor(vga16_text_t* vga, int16_t x, int16_t y) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  priv->cursor_x = x;
  priv->cursor_y = y;
}

void vga16_text_set_wrap(vga16_text_t* vga, bool w) {
  vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;
  priv->wrap = w;
}











// Métodos de acesso
//uint16_t vga16_text_get_width(const vga16_text_t* vga) {
//    return vga ? vga->width : 0;
//}

//uint16_t vga16_text_get_height(const vga16_text_t* vga) {
//    return vga ? vga->height : 0;
//}

//void vga16_text_set_text_color_big(vga16_text_t* vga, color_t color, color_t background) {
//  vga->set_text_color(vga, color);
//  vga->set_text_bg_color(vga,background);
//}
