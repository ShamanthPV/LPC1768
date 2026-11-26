#include <LPC17xx.h>
#include <stdint.h>

#define LED_PIN (1U << 29)

void delay_ms(uint32_t ms);

int main(void) {
    SystemInit();
    LPC_PINCON->PINSEL1 &= ~(3U << 18);
    LPC_GPIO1->FIODIR |= LED_PIN;

    while (1) {
        LPC_GPIO1->FIOSET = LED_PIN;
        delay_ms(500);

        LPC_GPIO1->FIOCLR = LED_PIN;
        delay_ms(500);
    }
}

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}