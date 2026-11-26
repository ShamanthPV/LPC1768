#include <LPC17xx.h>
#include <stdint.h>

void PWM1_Init(uint32_t period_us) {
    LPC_SC->PCONP |= (1U << 6);      /* Power ON PWM1 */
    LPC_SC->PCLKSEL0 &= ~(3U << 12); /* PCLK_PWM1 = 25 MHz */

    /* Configure P2.0 as PWM1.1 (PINSEL4 bits [1:0] = 01) */
    LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3U << 0)) | (1U << 0);

    LPC_PWM1->TCR = (1U << 1);       /* Reset PWM counter */
    LPC_PWM1->PR  = 25U - 1U;        /* 1 us tick @ 25 MHz */
    LPC_PWM1->MR0 = period_us;       /* PWM period */
    LPC_PWM1->MR1 = 0U;              /* Initial 0% duty */
    LPC_PWM1->MCR = (1U << 1);       /* Reset TC on MR0 match */
    LPC_PWM1->PCR = (1U << 9);       /* Enable PWM1.1 output */
    LPC_PWM1->LER = (1U << 0) | (1U << 1);
    LPC_PWM1->TCR = (1U << 0) | (1U << 3); /* Enable Counter + PWM mode */
}

void PWM1_SetDuty(uint32_t duty_us) {
    LPC_PWM1->MR1 = duty_us;
    LPC_PWM1->LER = (1U << 1); /* Latch new MR1 value on next period */
}

int main(void) {
    uint32_t duty;
    volatile uint32_t d;

    SystemInit();
    PWM1_Init(1000U); /* 1000 us = 1 kHz */

    while (1) {
        for (duty = 0U; duty <= 1000U; duty += 10U) {
            PWM1_SetDuty(duty);
            for (d = 0; d < 40000U; d++);
        }
        for (duty = 1000U; duty >= 10U; duty -= 10U) {
            PWM1_SetDuty(duty);
            for (d = 0; d < 40000U; d++);
        }
    }
}