#ifndef __VGA16_DRV_H__
#define __VGA16_DRV_H__

/**
 * Hunter Adams (vha3@cornell.edu)
 * modifed for 16 colors by BRL4
 * 
 *
 * HARDWARE CONNECTIONS
 *  - GPIO 16 ---> VGA Hsync
 *  - GPIO 17 ---> VGA Vsync
 *  - GPIO 18 ---> 470 ohm resistor ---> VGA Green 
 *  - GPIO 19 ---> 330 ohm resistor ---> VGA Green
 *  - GPIO 20 ---> 330 ohm resistor ---> VGA Blue
 *  - GPIO 21 ---> 330 ohm resistor ---> VGA Red
 *  - RP2040 GND ---> VGA GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1, 2, and 3
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 * NOTE
 *  - This is a translation of the display primitives
 *    for the PIC32 written by Bruce Land and students
 *
 */



// VGA primitives - usable in main
void initVGA(  char **active_buffer_ptr,unsigned int totalBytes) ;
char* get_vga_buffer_pointer(void);
//int get_width();
//int get_height();


// Length of the pixel array, and number of DMA transfers
//#define TXCOUNT 153600 // Total pixels/2 (since we have 2 pixels per byte)
//extern unsigned char vga_data_array[TXCOUNT];
#endif