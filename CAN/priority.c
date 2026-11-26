#include <LPC17xx.h>
#include <stdint.h>

#define CRITICAL_LED (1U << 25)
#define TELEM_LED    (1U << 26)

int main(void) {
    uint32_t rx_id;
    uint8_t payload;
    volatile uint32_t d;

    SystemInit();
    LPC_PINCON->PINSEL1 &= ~((3U << 18) | (3U << 20));
    LPC_GPIO0->FIODIR |= (CRITICAL_LED | TELEM_LED);

    LPC_SC->PCONP |= (1U << 13);
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~0x0FU) | 0x05U;
    LPC_CANAF->AFMR = 0x02U;
    LPC_CAN1->MOD   = 0x01U;
    LPC_CAN1->BTR   = 0x001C0009U; /* 500 kbps */
    LPC_CAN1->MOD   = 0x00U;

    while (1) {
        /* Broadcast periodic heartbeat from this node (ID 0x300) */
        if ((LPC_CAN1->SR & (1U << 2)) != 0U) {
            LPC_CAN1->TFI1 = (1U << 16);
            LPC_CAN1->TID1 = 0x300U;
            LPC_CAN1->TDA1 = 0x01U;
            LPC_CAN1->CMR  = 0x21U;
        }

        /* Dispatch incoming frames from multiple ECUs by CAN ID priority */
        if ((LPC_CAN1->SR & (1U << 0)) != 0U) {
            rx_id   = LPC_CAN1->RID & 0x7FFU;
            payload = (uint8_t)(LPC_CAN1->RDA & 0xFFU);
            LPC_CAN1->CMR = (1U << 2);

            if (rx_id == 0x100U) {
                /* High-Priority ECU Frame (e.g., Brake / Fault Node) */
                if (payload != 0U) LPC_GPIO0->FIOSET = CRITICAL_LED;
                else               LPC_GPIO0->FIOCLR = CRITICAL_LED;
            } else if (rx_id == 0x200U) {
                /* Medium-Priority ECU Frame (e.g., Sensor Node) */
                LPC_GPIO0->FIOPIN ^= TELEM_LED;
            }
        }

        for (d = 0; d < 100000U; d++);
    }
}