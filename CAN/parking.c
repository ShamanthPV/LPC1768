#include <LPC17xx.h>
#include <stdint.h>

#define TRIG_PIN (1U << 21)
#define ECHO_PIN (1U << 22)

void CAN1_Init(void) {
    LPC_SC->PCONP |= (1U << 13);
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~0x0FU) | 0x05U;
    LPC_CANAF->AFMR = 0x02U;
    LPC_CAN1->MOD   = 0x01U;
    LPC_CAN1->BTR   = 0x001C0009U; /* 500 kbps */
    LPC_CAN1->MOD   = 0x00U;
}

void CAN1_TxParking(uint8_t dist_cm, uint8_t alert_lvl) {
    while ((LPC_CAN1->SR & (1U << 2)) == 0U);
    LPC_CAN1->TFI1 = (2U << 16); /* Standard ID, DLC = 2 bytes */
    LPC_CAN1->TID1 = 0x101U;
    LPC_CAN1->TDA1 = ((uint32_t)alert_lvl << 8) | dist_cm;
    LPC_CAN1->CMR  = 0x21U;
}

uint8_t Measure_Cm(void) {
    uint32_t us, t = 50000U;
    volatile uint32_t k;

    LPC_GPIO0->FIOSET = TRIG_PIN;
    for (k = 0; k < 250U; k++);
    LPC_GPIO0->FIOCLR = TRIG_PIN;

    while (((LPC_GPIO0->FIOPIN & ECHO_PIN) == 0U) && (--t > 0U));
    if (t == 0U) return 255U;

    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->PR  = 25U - 1U;
    LPC_TIM0->TCR = 0x01;
    t = 50000U;
    while (((LPC_GPIO0->FIOPIN & ECHO_PIN) != 0U) && (--t > 0U));
    us = LPC_TIM0->TC;
    LPC_TIM0->TCR = 0x00;

    return (us / 58U > 250U) ? 250U : (uint8_t)(us / 58U);
}

int main(void) {
    uint8_t dist, alert;
    volatile uint32_t d;

    SystemInit();
    LPC_SC->PCONP |= (1U << 1);
    LPC_PINCON->PINSEL1 &= ~((3U << 10) | (3U << 12));
    LPC_GPIO0->FIODIR |= TRIG_PIN;
    LPC_GPIO0->FIODIR &= ~ECHO_PIN;
    CAN1_Init();

    while (1) {
        dist = Measure_Cm();
        alert = (dist < 15U) ? 2U : ((dist < 40U) ? 1U : 0U);
        CAN1_TxParking(dist, alert);
        for (d = 0; d < 250000U; d++);
    }
}