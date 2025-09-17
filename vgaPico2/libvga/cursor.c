#include "cursor.h"
#include <stdlib.h>

// Estrutura completa (privada)
struct cursor_t {
    // Atributos
    short x, y;
    bool visible;
    bool enabled;
    uint32_t blink_interval;
    repeating_timer_t timer;
    unsigned char fontsize;
    
    // Ponteiro para a função de callback (poderia ser expandido para polimorfismo)
    bool (*blink_callback)(repeating_timer_t *rt);
};

// Função de callback interna
static bool cursor_blink_callback_internal(repeating_timer_t *rt) {
    cursor_t *cursor = (cursor_t *)rt->user_data;
    if (cursor->enabled) {
        cursor->visible = !cursor->visible;
        
        // Redesenha o caractere na posição do cursor
        if (cursor->visible) {
          //  drawChar('_', WHITE, BLACK, cursor->fontsize);
        } else {
            // Salva e restaura o caractere original
          //  drawChar(' ', BLACK, BLACK, cursor->fontsize);
        }
    }
    return true;
}

// "Construtor"
cursor_t* cursor_create(short x, short y, uint32_t blink_interval, unsigned char fontsize) {
    cursor_t* cursor = (cursor_t*)malloc(sizeof(cursor_t));
    if (!cursor) return NULL;
    
    // Inicializa atributos
    cursor->x = x;
    cursor->y = y;
    cursor->visible = true;
    cursor->enabled = false;
    cursor->blink_interval = blink_interval;
    cursor->fontsize = fontsize;
    cursor->blink_callback = cursor_blink_callback_internal;
    
    return cursor;
}

// "Destrutor"
void cursor_destroy(cursor_t* cursor) {
    if (cursor) {
        // Para o timer se estiver ativo
        if (cursor->enabled) {
            cancel_repeating_timer(&cursor->timer);
        }
        free(cursor);
    }
}

void cursor_init(cursor_t* cursor) {
    if (!cursor) return;
    
    cursor_set_position(cursor, cursor->x, cursor->y);
    cursor_enable(cursor, true);
    
    add_repeating_timer_ms(cursor->blink_interval, 
                          cursor->blink_callback, 
                          cursor, 
                          &cursor->timer);
}

void cursor_set_position(cursor_t* cursor, short x, short y) {
    if (!cursor) return;
    
    // Apaga cursor na posição antiga
    if (cursor->enabled && cursor->visible) {
      //  drawChar(' ', BLACK, BLACK, cursor->fontsize);
    }
    
    // Nova posição
    cursor->x = x;
    cursor->y = y;
    cursor->visible = true;
    
    // Desenha cursor na nova posição se estiver habilitado
    if (cursor->enabled) {
    //    drawChar('_', WHITE, BLACK, cursor->fontsize);
    }
}

void cursor_enable(cursor_t* cursor, bool enable) {
    if (!cursor) return;
    
    cursor->enabled = enable;
    if (!enable && cursor->visible) {
        // Esconde o cursor quando desabilitado
    //    drawChar(' ', BLACK, BLACK, cursor->fontsize);
        cursor->visible = false;
    }
}

// Métodos de acesso (getters)
short cursor_get_x(const cursor_t* cursor) {
    return cursor ? cursor->x : 0;
}

short cursor_get_y(const cursor_t* cursor) {
    return cursor ? cursor->y : 0;
}

bool cursor_is_visible(const cursor_t* cursor) {
    return cursor ? cursor->visible : false;
}

bool cursor_is_enabled(const cursor_t* cursor) {
    return cursor ? cursor->enabled : false;
}


/* Exemplo de uso
#include "cursor.h"

int main() {
    // Cria uma instância do cursor
    cursor_t* my_cursor = cursor_create(10, 5, 500, 2);
    
    // Inicializa o cursor
    cursor_init(my_cursor);
    
    // Move o cursor
    cursor_set_position(my_cursor, 15, 8);
    
    // Desabilita temporariamente
    cursor_enable(my_cursor, false);
    
    // Reabilita
    cursor_enable(my_cursor, true);
    
    // Limpa a memória quando não for mais necessário
    cursor_destroy(my_cursor);
    
    return 0;
}



*/