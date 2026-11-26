#include <LPC17xx.h>
#include <stdint.h>

#define SENSOR_PIN 0x01000000 /* P0.24: switch input */
#define LED_PIN    0x20000000 /* P1.29: Status LED output */

void GPIO_Sensor_Init(void);
void delay_ms(uint32_t ms);

int main(void) {
    uint32_t pin_state;

    SystemInit();
    GPIO_Sensor_Init();

    while (1) {
        /* Read bit 24 from Port 0 Fast Pin Value Register (FIOPIN) */
        pin_state = (LPC_GPIO0->FIOPIN & SENSOR_PIN);

        /* Active-LOW switch logic:
         * P0.24 reads 0 when switch is pressed */
        if (pin_state == 0U) {
            LPC_GPIO1->FIOSET = LED_PIN; /* Turn LED ON */
        } else {
            LPC_GPIO1->FIOCLR = LED_PIN; /* Turn LED OFF */
        }

        delay_ms(20); /* Short debounce / polling interval */
    }
}

void GPIO_Sensor_Init(void) {
    /* 1. Configure P0.24 (bits [17:16]) and P0.25 (bits [19:18]) as GPIO in PINSEL1 */
    LPC_PINCON->PINSEL1 &= ~((0x00030000) | (0x000C0000));

    /* 2. Enable internal pull-up resistor on P0.24 (PINMODE1 bits [17:16] = 00)
     *    Prevents the input pin from floating when the sensor/switch is open */
    LPC_PINCON->PINMODE1 &= ~(0x00030000);

    /* 3. Set pin directions in FIODIR:
     *    Bit = 0 -> Input (P0.24), Bit = 1 -> Output (P1.29) */
    LPC_GPIO0->FIODIR &= ~SENSOR_PIN;
    LPC_GPIO1->FIODIR |= LED_PIN;

    /* 4. Start with LED OFF */
    LPC_GPIO1->FIOCLR = LED_PIN;
}

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}