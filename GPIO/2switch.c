#include <LPC17xx.h>
#include <stdint.h>

#define SW1_PIN  (1U << 23)
#define SW2_PIN  (1U << 24)
#define LED1_PIN (1U << 25)
#define LED2_PIN (1U << 26)

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

int main(void) {
    uint8_t led1_state = 0U;
    uint8_t led2_state = 0U;

    SystemInit();

    /* Configure P0.23..P0.26 as GPIO in PINSEL1 */
    LPC_PINCON->PINSEL1 &= ~((3U << 14) | (3U << 16) | (3U << 18) | (3U << 20));
    /* Enable internal pull-ups on P0.23 and P0.24 */
    LPC_PINCON->PINMODE1 &= ~((3U << 14) | (3U << 16));

    LPC_GPIO0->FIODIR &= ~(SW1_PIN | SW2_PIN);
    LPC_GPIO0->FIODIR |= (LED1_PIN | LED2_PIN);
    LPC_GPIO0->FIOCLR = (LED1_PIN | LED2_PIN);

    while (1) {
        if ((LPC_GPIO0->FIOPIN & SW1_PIN) == 0U) {
            delay_ms(30); /* Debounce */
            if ((LPC_GPIO0->FIOPIN & SW1_PIN) == 0U) {
                led1_state ^= 1U;
                if (led1_state) LPC_GPIO0->FIOSET = LED1_PIN;
                else            LPC_GPIO0->FIOCLR = LED1_PIN;
                while ((LPC_GPIO0->FIOPIN & SW1_PIN) == 0U);
            }
        }

        if ((LPC_GPIO0->FIOPIN & SW2_PIN) == 0U) {
            delay_ms(30);
            if ((LPC_GPIO0->FIOPIN & SW2_PIN) == 0U) {
                led2_state ^= 1U;
                if (led2_state) LPC_GPIO0->FIOSET = LED2_PIN;
                else            LPC_GPIO0->FIOCLR = LED2_PIN;
                while ((LPC_GPIO0->FIOPIN & SW2_PIN) == 0U);
            }
        }
    }
}