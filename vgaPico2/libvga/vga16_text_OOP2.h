#ifndef VGA16_TEXT_H
#define VGA16_TEXT_H

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "colors.h"
#include <stdint.h>

// Estrutura forward declaration
typedef struct vga16_text_t vga16_text_t;
// For accessing the font library
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

// Construtor e Destrutor
vga16_text_t* vga16_text_create(uint8_t mode);
void vga16_text_destroy(vga16_text_t* vga);

// Definição da estrutura com métodos
struct vga16_text_t {
    // Campos públicos (se necessário)
    uint16_t width;
    uint16_t height;

    // Métodos
    void (*init)(vga16_text_t* self, uint8_t mode);
    void (*clrscr)(vga16_text_t* self);
    void (*pchar)(vga16_text_t* self, char c);
    void (*draw_pixel)(vga16_text_t* self, int16_t x, int16_t y, color_t color);
    void (*draw_hline)(vga16_text_t* vga, int16_t x, int16_t y, int16_t w, color_t color);
    void (*fill_rect)(vga16_text_t* vga, int16_t x, int16_t y, int16_t w, int16_t h, color_t color);
    void (*draw_char_interna)(vga16_text_t* vga, uint8_t c);
    void (*draw_char)(vga16_text_t* vga, uint8_t c);
    void (*print_string)(vga16_text_t* self, char* str);
    void (*tft_write)(vga16_text_t* vga, uint8_t c);
    void (*set_text_color_big)(vga16_text_t* vga, color_t color, color_t background);
    void (*write_string_bold)(vga16_text_t* vga, int8_t* str);
    color_t (*read_pixel)(const vga16_text_t* vga, int16_t x, int16_t y);

    // Getters
    uint8_t  (*get_text_size)(const vga16_text_t* vga);
    color_t  (*get_text_bgcolor)(const vga16_text_t* vga);

    // Setters
    void (*set_text_size)(const vga16_text_t* vga, uint8_t new_textsize);
    void (*set_text_color)(vga16_text_t* self, color_t c);
    void (*set_text_color2)(vga16_text_t* vga, color_t c, color_t b);
    void (*set_text_bgcolor)(vga16_text_t* self, color_t c);
    void (*set_text_cursor)(vga16_text_t* self, int16_t x, int16_t y);
    void (*set_text_wrap)(vga16_text_t* vga, bool w);





//    uint16_t (*get_cursor_x)(vga16_text_t* self);
//    uint16_t (*get_cursor_y)(vga16_text_t* self);
//    void (*set_cursor_x)(vga16_text_t* self, int16_t new_x);
//    void (*set_cursor_y)(vga16_text_t* self, int16_t new_y);
//    void (*set_width)(vga16_text_t* self, int16_t new_width);
//    void (*set_height)(vga16_text_t* self, int16_t new_height);
//    uint16_t (*get_width)(vga16_text_t* self);
//    uint16_t (*get_height)(vga16_text_t* self);


    void* _private;
};


 
#endif