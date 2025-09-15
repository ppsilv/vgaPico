
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "string.h"
#include "vga16_drv.h"
#include "vga16_text.h"
// protothreads header
#include "pt_cornell_rp2040_v1_4.h"

#ifndef WHITE
    #define BLACK 0
    #define DARK_GREEN 1
    #define MED_GREEN 2
    #define GREEN 3
    #define DARK_BLUE 4
    #define BLUE 5
    #define LIGHT_BLUE 6
    #define CYAN 7
    #define RED 8
    #define DARK_ORANGE 9
    #define ORANGE 10
    #define YELLOW 11
    #define MAGENTA 12
    #define PINK 13
    #define LIGHT_PINK 14
    #define WHITE 15
#endif

#include "colors.h"


// restart graphics flag
bool restart_graphics = true ;
// shared variable for erase color
int bkgnd_color = 0 ;

int msprintf(char *str, const char *format, ...);

// ==================================================
// === toggle25 thread on core 0
// ==================================================
// the on-board LED blinks
static PT_THREAD (protothread_toggle25(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;
    
     // set up LED p25 to blink
     gpio_init(25) ;	
     gpio_set_dir(25, GPIO_OUT) ;
     gpio_put(25, true);
     // data structure for interval timer
     PT_INTERVAL_INIT() ;

      while(1) {
        // yield time 0.1 second
        //PT_YIELD_usec(100000) ;
        PT_YIELD_INTERVAL(100000) ;

        // toggle the LED on PICO
        LED_state = LED_state? false : true ;
        gpio_put(25, LED_state);
        //
        // NEVER exit while
      } // END WHILE(1)
  PT_END(pt);
} // blink thread

 volatile   int cursorx=0;
 volatile   int cursory=0;


static void pwriteStr(char *str) {
    while(*str) {
        pchar(*str++);
    } 
}

static PT_THREAD (protothread_write_screen(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;
    char b[256];    // data structure for interval timer
     PT_INTERVAL_INIT() ;


    // Write some text
    setTextColor(WHITE) ;
    setTextSize(2) ;
    
   // writeString("Teste de escrita na tela") ;
      volatile unsigned char i=1,j=1;
      while(1) {
        
        PT_YIELD_INTERVAL(100000) ;
       
        //msprintf(b, "%02x", i);
        //pwriteStr(b);
        ////pchar('1') ;
        //i++ ;
        //if(i>=254) {
        //  i=0;
        //  j++;
        //  if (j>3){ 
        //    j=0;
        //    clrscr();
        //  } 
        //}
        //cursorx = get_cursor_x();
        //cursory = get_cursor_y();

        setTextCursor( 10 , 200);
        pchar('A') ;
        //if(cursory>100) {
        //  clrscr();
        //  continue ;
        //}
        

        setTextCursor( 10 , 400);
        msprintf(b, "X=%03d Y=%03d ", i, j);
        printString(b);

        setTextCursor( cursorx , cursory);


      } // END WHILE(1)
  PT_END(pt);
} // blink thread


// ========================================
// === core 0 main
// ========================================
int main(){
  // set the clock
  set_sys_clock_khz(150000, true);

  // start the serial i/o
  stdio_init_all() ;
  
  // announce the threader version on system reset
  printf("\n\rProtothreads RP2040/2350 v1.4 \n\r");

  // Initialize the VGA screen
  initVGA() ;
  //printf("video assumes cpu clock=%d MHz", rgb_vid_freq);

  init_text_screen(1);
  // start core 1 threads
 // multicore_reset_core1();
  //multicore_launch_core1(&core1_main);

  // === config threads ========================
  // for core 0
  pt_add_thread(protothread_write_screen);
  //pt_add_thread(protothread_graphics);
  pt_add_thread(protothread_toggle25);
  //
  // === initalize the scheduler ===============
  pt_schedule_start ;
  // NEVER exits
  // ===========================================
} // end main

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

int msprintf(char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int i = 0, j = 0;
    char buffer[20];
    char padding_char;
    int width;
    
    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++; // Pula o '%'
            
            // Verifica se há preenchimento com zero
            padding_char = ' ';
            if (format[i] == '0') {
                padding_char = '0';
                i++;
            }
            
            // Largura do campo
            width = 0;
            while (isdigit(format[i])) {
                width = width * 10 + (format[i] - '0');
                i++;
            }
            
            // Processa o especificador de formato
            switch (format[i]) {
                case 'd': { // Decimal
                    int num = va_arg(args, int);
                    int len = snprintf(buffer, sizeof(buffer), "%d", num);
                    
                    // Aplica preenchimento se necessário
                    if (width > len) {
                        for (int k = 0; k < width - len; k++) {
                            str[j++] = padding_char;
                        }
                    }
                    
                    // Copia o número
                    for (int k = 0; k < len; k++) {
                        str[j++] = buffer[k];
                    }
                    break;
                }
                
                case 'x': // Hexadecimal minúsculo
                case 'X': { // Hexadecimal maiúsculo
                    unsigned int num = va_arg(args, unsigned int);
                    int is_upper = (format[i] == 'X');
                    int len = snprintf(buffer, sizeof(buffer), is_upper ? "%X" : "%x", num);
                    
                    // Aplica preenchimento se necessário
                    if (width > len) {
                        for (int k = 0; k < width - len; k++) {
                            str[j++] = padding_char;
                        }
                    }
                    
                    // Copia o número hexadecimal
                    for (int k = 0; k < len; k++) {
                        str[j++] = buffer[k];
                    }
                    break;
                }
                
                case 'c': { // Caractere
                    char c = (char)va_arg(args, int);
                    
                    // Aplica preenchimento se necessário
                    if (width > 1) {
                        for (int k = 0; k < width - 1; k++) {
                            str[j++] = padding_char;
                        }
                    }
                    
                    str[j++] = c;
                    break;
                }
                
                case 's': { // String
                    char *s = va_arg(args, char*);
                    int len = strlen(s);
                    
                    // Aplica preenchimento se necessário
                    if (width > len) {
                        for (int k = 0; k < width - len; k++) {
                            str[j++] = padding_char;
                        }
                    }
                    
                    // Copia a string
                    for (int k = 0; k < len; k++) {
                        str[j++] = s[k];
                    }
                    break;
                }
                
                default:
                    // Se não for um especificador reconhecido, copia o caractere
                    str[j++] = format[i];
                    break;
            }
            i++;
        } else {
            str[j++] = format[i++];
        }
    }
    
    str[j] = '\0'; // Termina a string
    va_end(args);
    
    return j; // Retorna o número de caracteres escritos
}
/*
// Exemplo de uso
int lixo() {
    char resultado[100];
    
    // Teste com diferentes formatadores
    meu_sprintf(resultado, "Decimal: %08d, Hex: 0x%04X, Char: %c, String: %s", 
                123, 255, 'A', "Hello");
    
    printf("Resultado: %s\n", resultado);
    
    // Outro teste
    char buffer[50];
    meu_sprintf(buffer, "%05d %03X %c %-10s", 42, 15, 'Z', "test");
    printf("Outro: %s\n", buffer);
    
    return 0;
}
*/