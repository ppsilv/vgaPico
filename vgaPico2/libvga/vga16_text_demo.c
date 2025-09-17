
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

//vga16_text_t * vga = NULL ;

// restart graphics flag
bool restart_graphics = true ;
// shared variable for erase color
int bkgnd_color = 0 ;

int msprintf(char *str, const char *format, ...);

vga16_text_t *vga = NULL ;

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
static PT_THREAD (protothread_blinkCursor(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;

      //Init something here


     PT_INTERVAL_INIT() ;

      while(1) {
        put_cursor(1);
        PT_YIELD_INTERVAL(500000) ;
        put_cursor(0);
        PT_YIELD_INTERVAL(500000) ;
      } // END WHILE(1)
  PT_END(pt);
} // blink thread

static PT_THREAD (protothread_controlblinkCursor(struct pt *pt))
{
    PT_BEGIN(pt);
    static int control=0;
      //Init something here
   // vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;

     PT_INTERVAL_INIT() ;

      while(1) {
        control++;
        PT_YIELD_INTERVAL(5000000) ;
        switch(control){
          case 1:
     //           priv->cursor->blink=false;
                break;
          case 2:
     //           priv->cursor->visible=false;
                break;
          case 3:                      
     //           priv->cursor->blink=true;
                break;
          case 4:
     //           priv->cursor->visible=true;
                control=0;
                break;
        }
      } // END WHILE(1)
  PT_END(pt);
} // blink thread

 volatile   int cursorx=0;
 volatile   int cursory=0;


static void pwriteStr(char *str) {
    while(*str) {
      //  vga16_text_pchar(vga,*str++);
    } 
}
/*
static PT_THREAD (protothread_write_screen(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;
    char b[256];    // data structure for interval timer
     PT_INTERVAL_INIT(); 
    
    vga16_text_set_text_cursor(vga, 10, 120);
    vga16_text_print_string(vga, "Thread write screen");

   // writeString("Teste de escrita na tela") ;
      volatile unsigned char i=1,j=1;
      while(1) {        
        PT_YIELD_INTERVAL(500000) ;

  //      sleep_ms(500);
        vga16_text_set_text_cursor(vga, 10, 460);
        vga16_text_print_string(vga, "Hello World! ");
//        sleep_ms(500);
        PT_YIELD_INTERVAL(500000) ;

        vga16_text_set_text_cursor(vga, 10, 460);
        vga16_text_print_string(vga, "            ");
      } // END WHILE(1)

    PT_END(pt);
} // blink thread
*/
// ========================================
// === core 0 main
// ========================================
int main(){

    // set the clock
    set_sys_clock_khz(150000, true);

    // start the serial i/o
    stdio_init_all() ;
  
    // Initialize the VGA screen
    initVGA() ;

    vga = create_screen(MODE_640x480);

    vga->setTextColor(WHITE, BLACK);
    
    vga->setTextSize(1);

    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->setTextSize(2);
    vga->printString("         0         1         2         3");      
    vga->printString("1234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3");      
    vga->printString("1234567890123456789012345678901234567890");
    vga->printString("         0         1\n");      
    vga->printString("12345678901234567890");
    vga->clrscr();
    vga->printString("Paulo da silva (c) 2025...\n");
    vga->setTextCursor(10,20);
    vga->printString("Paulo");
    vga->setTextCursor(20,20);
    vga->printString(" Silva");


  // === config threads ========================
  // for core 0
  pt_add_thread(protothread_blinkCursor);

  pt_add_thread(protothread_toggle25);

  // === initalize the scheduler ===============
  pt_schedule_start ;
  // NEVER exits
  // ===========================================
} // end main
