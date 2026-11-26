#include <LPC17xx.h>
#include <stdint.h>

#define LED_PIN (1U << 25)

void Timer0_Delay5Sec(void) {
    LPC_SC->PCONP |= (1U << 1);       /* Power ON Timer0 */
    LPC_SC->PCLKSEL0 &= ~(3U << 2);   /* PCLK_TIMER0 = CCLK/4 = 25 MHz */

    LPC_TIM0->TCR = 0x02;             /* Reset Timer */
    LPC_TIM0->PR  = 25000U - 1U;      /* 25 MHz / 25000 = 1 kHz (1 ms tick) */
    LPC_TIM0->MR0 = 5000U - 1U;       /* 5000 ms = 5 seconds */
    LPC_TIM0->MCR = (1U << 0) | (1U << 2); /* Interrupt flag + Stop on MR0 */
    LPC_TIM0->IR  = 0xFF;             /* Clear pending interrupts */
    LPC_TIM0->TCR = 0x01;             /* Start Timer0 */

    while ((LPC_TIM0->IR & 0x01U) == 0U);
    LPC_TIM0->IR = 0x01U;
}

int main(void) {
    SystemInit();
    LPC_PINCON->PINSEL1 &= ~(3U << 18);
    LPC_GPIO0->FIODIR |= LED_PIN;

    while (1) {
        LPC_GPIO0->FIOSET = LED_PIN;
        Timer0_Delay5Sec();
        LPC_GPIO0->FIOCLR = LED_PIN;
        Timer0_Delay5Sec();
    }
}