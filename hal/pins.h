#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

/* RED LED = D3 (led2 i din Wokwi) */
#define LED_DDR   DDRD
#define LED_PORT  PORTD
#define LED_PIN   PD3

/* GREEN LED = D2 (led1 i din Wokwi) */
#define GREEN_DDR  DDRD
#define GREEN_PORT PORTD
#define GREEN_PIN  PD2

#endif