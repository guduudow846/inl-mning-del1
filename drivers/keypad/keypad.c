#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "keypad.h"

/*
    4x4 keypad
    Rader:    PD2, PD3, PD4, PD5   -> outputs
    Kolumner: PB0, PB1, PB2, PB3   -> inputs with pull-up

    Layout:
    1 2 3 A
    4 5 6 B
    7 8 9 C
    * 0 # D
*/

static const char keypad_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void keypad_init(void)
{
    // RADER = PB0–PB3 (outputs)
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);

    // KOLUMNER = PD4–PD7 (inputs)
    DDRD &= ~((1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
    PORTD |= (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
}

static void keypad_set_all_rows_high(void)
{
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
}

static void keypad_set_row_low(uint8_t row)
{
    keypad_set_all_rows_high();

    switch (row)
    {
        case 0: PORTB &= ~(1 << PB3); break; // R1 (pin 11)
        case 1: PORTB &= ~(1 << PB2); break; // R2 (pin 10)
        case 2: PORTB &= ~(1 << PB1); break; // R3 (pin 9)
        case 3: PORTB &= ~(1 << PB0); break; // R4 (pin 8)
    }
}

static uint8_t keypad_read_column(void)
{
    if (!(PIND & (1 << PD7))) return 0; // C1 (pin 7)
    if (!(PIND & (1 << PD6))) return 1; // C2 (pin 6)
    if (!(PIND & (1 << PD5))) return 2; // C3 (pin 5)
    if (!(PIND & (1 << PD4))) return 3; // C4 (pin 4)

    return 0xFF;
}

char keypad_get_key(void)
{
    uint8_t row;
    uint8_t col;

    for (row = 0; row < 4; row++)
    {
        keypad_set_row_low(row);

        /* kort stabiliseringstid */
        _delay_us(5);

        col = keypad_read_column();
        if (col != 0xFF)
        {
            return keypad_map[row][col];
        }
    }

    return 0;
}

char keypad_get_key_debounced(void)
{
    char key = keypad_get_key();

    if (key != 0)
    {
        _delay_ms(20);

        if (key == keypad_get_key())
        {
            /* vänta tills knappen släpps */
            while (keypad_get_key() != 0)
            {
                _delay_ms(5);
            }

            return key;
        }
    }

    return 0;
}