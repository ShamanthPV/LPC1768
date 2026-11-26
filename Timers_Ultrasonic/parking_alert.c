#include <LPC17xx.h>
#include <stdint.h>

#define TRIG_PIN   (1U << 21)
#define ECHO_PIN   (1U << 22)
#define BUZZER_PIN (1U << 25)

void delay_us(uint32_t us) {
    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->PR  = 25U - 1U; /* 1 us tick @ 25 MHz PCLK */
    LPC_TIM0->TCR = 0x01;
    while (LPC_TIM0->TC < us);
    LPC_TIM0->TCR = 0x00;
}

void delay_ms(uint32_t ms) {
    uint32_t i;
    for (i = 0; i < ms; i++) delay_us(1000U);
}

uint32_t Ultrasonic_GetDistanceCm(void) {
    uint32_t pulse_us;
    uint32_t timeout = 60000U;

    LPC_GPIO0->FIOSET = TRIG_PIN;
    delay_us(10U);
    LPC_GPIO0->FIOCLR = TRIG_PIN;

    while (((LPC_GPIO0->FIOPIN & ECHO_PIN) == 0U) && (--timeout > 0U));
    if (timeout == 0U) return 999U;

    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->PR  = 25U - 1U; /* 1 us per count */
    LPC_TIM0->TCR = 0x01;

    timeout = 60000U;
    while (((LPC_GPIO0->FIOPIN & ECHO_PIN) != 0U) && (--timeout > 0U));
    pulse_us = LPC_TIM0->TC;
    LPC_TIM0->TCR = 0x00;

    return pulse_us / 58U; /* Distance in cm */
}

int main(void) {
    uint32_t dist_cm;

    SystemInit();
    LPC_SC->PCONP |= (1U << 1);
    LPC_PINCON->PINSEL1 &= ~((3U << 10) | (3U << 12) | (3U << 18));
    LPC_GPIO0->FIODIR |= (TRIG_PIN | BUZZER_PIN);
    LPC_GPIO0->FIODIR &= ~ECHO_PIN;

    while (1) {
        dist_cm = Ultrasonic_GetDistanceCm();

        /* Sl.No 21 Parking Sensor Beep Logic */
        if (dist_cm < 10U) {
            LPC_GPIO0->FIOSET = BUZZER_PIN; /* Continuous alarm */
            delay_ms(50U);
        } else if (dist_cm <= 35U) {
            LPC_GPIO0->FIOSET = BUZZER_PIN;
            delay_ms(dist_cm * 4U);
            LPC_GPIO0->FIOCLR = BUZZER_PIN;
            delay_ms(dist_cm * 4U);
        } else {
            LPC_GPIO0->FIOCLR = BUZZER_PIN; /* Safe distance */
            delay_ms(100U);
        }
    }
}