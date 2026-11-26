#include <LPC17xx.h>
#include <stdint.h>

#define IN1_PIN (1U << 24)
#define IN2_PIN (1U << 25)

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

void Motor_PWM_Init(void) {
    LPC_SC->PCONP |= (1U << 6);
    LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3U << 0)) | (1U << 0);
    LPC_PINCON->PINSEL1 &= ~((3U << 16) | (3U << 18));
    LPC_GPIO0->FIODIR |= (IN1_PIN | IN2_PIN);

    LPC_PWM1->TCR = (1U << 1);
    LPC_PWM1->PR  = 25U - 1U;
    LPC_PWM1->MR0 = 1000U; /* 1 kHz PWM */
    LPC_PWM1->MCR = (1U << 1);
    LPC_PWM1->PCR = (1U << 9);
    LPC_PWM1->LER = (1U << 0);
    LPC_PWM1->TCR = (1U << 0) | (1U << 3);
}

void Motor_Drive(int8_t dir, uint16_t speed_percent) {
    if (speed_percent > 100U) speed_percent = 100U;

    if (dir > 0) {
        LPC_GPIO0->FIOSET = IN1_PIN;
        LPC_GPIO0->FIOCLR = IN2_PIN;
    } else if (dir < 0) {
        LPC_GPIO0->FIOCLR = IN1_PIN;
        LPC_GPIO0->FIOSET = IN2_PIN;
    } else {
        LPC_GPIO0->FIOCLR = (IN1_PIN | IN2_PIN);
    }

    LPC_PWM1->MR1 = (uint32_t)speed_percent * 10U;
    LPC_PWM1->LER = (1U << 1);
}

int main(void) {
    SystemInit();
    Motor_PWM_Init();

    while (1) {
        Motor_Drive(1, 35U);  delay_ms(2000); /* Forward 35% */
        Motor_Drive(1, 85U);  delay_ms(2000); /* Forward 85% */
        Motor_Drive(0, 0U);   delay_ms(1000); /* Stop */
        Motor_Drive(-1, 60U); delay_ms(2000); /* Reverse 60% */
        Motor_Drive(0, 0U);   delay_ms(1000);
    }
}