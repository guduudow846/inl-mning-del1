#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>

#include "app.h"
#include "keypad.h"

/* ===== LEDs ===== */
#define GREEN_LED PD2   // D2
#define RED_LED   PD3   // D3

/* ===== Start button ===== */
#define START_BUTTON PB4   // D12

/* ===== PIN ===== */
#define PIN_CODE "1772"
#define PIN_LEN 4

typedef enum
{
    IDLE,
    INPUT,
    ACCESS_GRANTED

} state_t;

static state_t state = IDLE;

static char entered_pin[PIN_LEN + 1];
static uint8_t pin_index = 0;



static void red_on(void)
{
    PORTD |= (1 << RED_LED);
}

static void red_off(void)
{
    PORTD &= ~(1 << RED_LED);
}

static void green_on(void)
{
    PORTD |= (1 << GREEN_LED);
}

static void green_off(void)
{
    PORTD &= ~(1 << GREEN_LED);
}

static void green_flash(void)
{
    green_on();
    _delay_ms(80);
    green_off();
}



void app_init(void)
{
    /* LEDs output */
    DDRD |= (1 << GREEN_LED) | (1 << RED_LED);

    /* pushbutton input */
    DDRB &= ~(1 << START_BUTTON);
    PORTB |= (1 << START_BUTTON);

    keypad_init();

    red_on();
    green_off();
}



void app_run(void)
{
    static uint16_t blink_counter = 0;
    static uint32_t timeout_counter = 0;

    char key;

    switch (state)
    {
        

        case IDLE:

            red_on();
            green_off();

            pin_index = 0;
            memset(entered_pin, 0, sizeof(entered_pin));

            
            if (!(PINB & (1 << START_BUTTON)))
            {
                state = INPUT;

                blink_counter = 0;
                timeout_counter = 0;

                _delay_ms(200);
            }

            break;

        

        case INPUT:

            /* red blink */
            if (blink_counter >= 200)
            {
                PORTD ^= (1 << RED_LED);
                blink_counter = 0;
            }

            _delay_ms(1);

            blink_counter++;
            timeout_counter++;

            /* timeout 5 sec */
            if (timeout_counter >= 5000)
            {
                state = IDLE;
                break;
            }

            key = keypad_get_key_debounced();

            if (key)
            {
                green_flash();

                if (pin_index < PIN_LEN)
                {
                    entered_pin[pin_index++] = key;
                }
            }

            /* check pin */
            if (pin_index >= PIN_LEN)
            {
                entered_pin[PIN_LEN] = '\0';

                if (strcmp(entered_pin, PIN_CODE) == 0)
                {
                    state = ACCESS_GRANTED;
                }
                else
                {
                    state = IDLE;
                }
            }

            break;

        /* ================= ACCESS ================= */

        case ACCESS_GRANTED:

            red_off();
            green_on();

            _delay_ms(3000);

            state = IDLE;

            break;
    }
}