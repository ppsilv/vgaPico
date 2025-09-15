/**
 * Hunter Adams (vha3@cornell.edu)
 * 
 * HARDWARE CONNECTIONS
 *  
- GPIO 16 ---> VGA Hsync
- GPIO 17 ---> VGA Vsync
- GPIO 18 ---> VGA Green lo-bit --> 470 ohm resistor --> VGA_Green
- GPIO 19 ---> VGA Green hi_bit --> 330 ohm resistor --> VGA_Green
- GPIO 20 ---> 330 ohm resistor ---> VGA-Blue
- GPIO 21 ---> 330 ohm resistor ---> VGA-Red
- RP2040 GND ---> VGA-GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1, 2, and 3
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 * Protothreads v1.4
 * Threads:
 * core 0:
 * Graphics demo
 * blink LED25 
 * core 1:
 * Serial i/o 
 */
// ==========================================
// === VGA graphics library
// ==========================================
#include "vga16_graphics_v2.h"
#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
//
#include "hardware/vreg.h"
#include "hardware/clocks.h"

// ==========================================
// === protothreads globals
// ==========================================
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "string.h"
// protothreads header
#include "pt_cornell_rp2040_v1_4.h"
//
// restart graphics flag
bool restart_graphics = true ;
// shared variable for erase color
int bkgnd_color = 0 ;


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

    int cursorx=0;
    int cursory=0;


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
    setTextSize(1) ;
    
   // writeString("Teste de escrita na tela") ;

      while(1) {
        // yield time 0.001 second
        //PT_YIELD_usec(1000) ;
        PT_YIELD_INTERVAL(100000) ;
        pchar('P') ;

        cursorx = get_cursor_x();
        cursory = get_cursor_y();

        setCursor( 100 , 100);
        sprintf(b, "X=%03d                       Y=%03d ", cursorx, cursory);
        writeString(b);

        setCursor( cursorx , cursory);


      } // END WHILE(1)
  PT_END(pt);
} // blink thread

// ==================================================
// === user's serial input thread on core 1
// ==================================================
// serial_read an serial_write do not block any thread
// except this one
static PT_THREAD (protothread_serial(struct pt *pt))
{
    PT_BEGIN(pt);
      static uint64_t e_time ;

      while(1) {
        // print prompt
        sprintf(pt_serial_out_buffer, "erase/restart: ");
        // spawn a thread to do the non-blocking write
        serial_write ;

        // spawn a thread to do the non-blocking serial read
        serial_read ;
        // convert input string to number
        //sscanf(pt_serial_in_buffer,"%d %d", &test_in1, &test_in2) ;
        short color;
        if(pt_serial_in_buffer[0]=='e'){
          sscanf(pt_serial_in_buffer+1,"%d\r", &bkgnd_color) ;
          // wipe the whole frame
          e_time = PT_GET_TIME_usec();
          clearLowFrame(0, (short)bkgnd_color) ;
          printf( "Erase Time %lld uSec\n\r", (PT_GET_TIME_usec() - e_time));
        }

        if(pt_serial_in_buffer[0]=='r'){
          restart_graphics = true;
        }

        // NEVER exit while
      } // END WHILE(1)
  PT_END(pt);
} // serial thread

// ========================================
// === core 1 main -- started in main below
// ========================================
void core1_main(){ 
  //
  //  === add threads  ====================
  // for core 1
  pt_add_thread(protothread_serial) ;
  //
  // === initalize the scheduler ==========
  pt_schedule_start ;
  // NEVER exits
  // ======================================
}

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

