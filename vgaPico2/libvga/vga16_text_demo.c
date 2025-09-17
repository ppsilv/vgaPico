
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

repeating_timer_t timer;
static int last_toggle_time = 1;

static bool timer_callback(repeating_timer_t *rt)
{
    if(last_toggle_time == 1){
        put_cursor(1);
        last_toggle_time = 0;
    }
    else{
        put_cursor(0);
        last_toggle_time = 1;
    }
  return true;
}
static void create_timer()
{
    cancel_repeating_timer(&timer);
    int16_t tempo = vga->get_blink_interval();
    add_repeating_timer_ms(tempo, timer_callback, NULL, &timer);
}
static void pwriteStr(char *str) {
    while(*str) {
      //  vga16_text_pchar(vga,*str++);
    } 
}
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
    static int control=4;
      //Init something here
   // vga16_text_private_t* priv = (vga16_text_private_t*)vga->_private;

     PT_INTERVAL_INIT() ;

      while(1) {
        control++;
        PT_YIELD_INTERVAL(5000000) ;
        switch(control){
          case 1:
              vga->pchar('A');
              vga->setTextCursorBlink(false);
              break;
          case 2:
              vga->setTextCursorVisible(false);
              break;
          case 3:                      
              vga->setTextCursorBlink(true);
              break;
          case 4:
              vga->setTextCursorVisible(true);
              break;
          case 5:
              vga->set_blink_interval(75);
              vga->pchar('b');
              create_timer();              
              break;
          case 6:
              vga->set_blink_interval(150);
              vga->pchar('c');
              create_timer();              
              break;
          case 7:
              vga->set_blink_interval(450);
              vga->pchar('d');
              create_timer();              
              break;
          case 8:
              vga->set_blink_interval(1000);
              vga->pchar('e');
              create_timer();              
              control=0;
              break;
        }
      } // END WHILE(1)
  PT_END(pt);
} // blink thread

 volatile   int cursorx=0;
 volatile   int cursory=0;

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
void swap_buffers(char **active_buffer_ptr, unsigned char *new_buffer) {
    *active_buffer_ptr = (char *)new_buffer;
    
    // Opcional: esperar a DMA terminar antes de trocar
    // dma_channel_wait_for_finish_blocking(rgb_chan_0);
}
#define TXCOUNT 153600 // Total pixels/2 (since we have 2 pixels per byte)
char vga_video_data_array0[TXCOUNT];
char vga_video_data_array1[TXCOUNT];
char *active_buffer = (char *)&vga_video_data_array0[0];
static PT_THREAD (protothread_trocatela(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;

      //Init something here


     PT_INTERVAL_INIT() ;

      while(1) {
        PT_YIELD_INTERVAL(5000000) ;
        swap_buffers(&active_buffer, vga_video_data_array0);
        vga->set_vga_data_array(vga_video_data_array0);
        PT_YIELD_INTERVAL(5000000) ;
        swap_buffers(&active_buffer, vga_video_data_array1);
        vga->set_vga_data_array(vga_video_data_array1);
      } // END WHILE(1)
  PT_END(pt);
} // blink thread

int main(){

    // set the clock
    set_sys_clock_khz(150000, true);

    // start the serial i/o
    stdio_init_all() ;
  
    // Initialize the VGA screen
    initVGA(  &active_buffer, TXCOUNT ) ;

    vga = create_screen( MODE_640x480, active_buffer, TXCOUNT );
    vga->set_vga_data_array(vga_video_data_array0);
    vga->setTextColor(WHITE, BLACK);
    vga->setTextSize(1);
    // Configura o timer para chamar a callback a cada 500ms
    add_repeating_timer_ms(250, timer_callback, NULL, &timer);

    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    vga->printString("         0         1         2         3         4         5         6         7");      
    vga->printString("1234567890123456789012345678901234567890123456789012345678901234567890");
    vga->setTextCursor(10,4);
    vga->printString("Tela numero 1");
    //vga->clrscr();
    swap_buffers(&active_buffer, vga_video_data_array1);
    vga->set_vga_data_array(vga_video_data_array1);

    vga->printString("Paulo da silva (c) 2025...\n");
    vga->setTextCursor(10,20);
    vga->printString("Paulo");
    vga->setTextCursor(20,20);
    vga->printString(" Silva");


  // === config threads ========================
  // for core 0
//  pt_add_thread(protothread_blinkCursor);

  pt_add_thread(protothread_trocatela);
 

  pt_add_thread(protothread_toggle25);

  // === initalize the scheduler ===============
  pt_schedule_start ;
  // NEVER exits
  // ===========================================
} // end main
