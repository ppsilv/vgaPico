#include "pico/stdlib.h"
#include "vga16_text.h"
#include "cursor.h"
#include "colors.h"

cursor_t cursor ;
cursor_t * text_cursor =NULL;

bool cursor_blink_callback(repeating_timer_t *rt) {
    cursor_t *cursor = (cursor_t *)rt->user_data;
    if (cursor->enabled) {
        cursor->visible = !cursor->visible;
        
        // Redesenha o caractere na posição do cursor
        if (cursor->visible) {
            drawChar( 'A', WHITE, BLACK, 2);
        } else {
            drawChar( 'B', BLACK, BLACK, 2);
        }
    }
    return true;
}

void cursor_init(cursor_t *cursor) {
    text_cursor = cursor ;
    cursor_set_position(get_cursor_x(), get_cursor_y() );
    cursor_enable(true);
    add_repeating_timer_ms(cursor->blink_interval, 
                          cursor_blink_callback, 
                          cursor, 
                          &cursor->timer);


}

void cursor_set_position(short x, short y) {
    // Apaga cursor na posição antiga
 //   drawChar(' ', BLACK, BLACK,  text_cursor->fontsize);
    printString("MAS QUE MERDA!!! ") ;
    // Nova posição
    text_cursor->x = x;
    text_cursor->y = y;
    text_cursor->visible = true;
    
    // Desenha cursor na nova posição
    drawChar('C', WHITE, BLACK,  text_cursor->fontsize);
}

void cursor_enable(bool enable) {
    text_cursor->enabled = enable;
    if (!enable) {
        // Esconde o cursor quando desabilitado
        drawChar( ' ', BLACK, BLACK, text_cursor->fontsize);
    }
}