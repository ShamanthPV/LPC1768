#include <LPC17xx.h>
#include <stdint.h>

void CAN1_Init(void) {
    LPC_SC->PCONP |= (1U << 13); /* PCAN1 */
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~0x0FU) | 0x05U; /* P0.0 RD1, P0.1 TD1 */

    LPC_CANAF->AFMR = 0x02U;     /* Acceptance Filter Bypass Mode (accept all IDs) */
    LPC_CAN1->MOD   = 0x01U;     /* Reset Mode */
    /* 25 MHz PCLK / (10 * (1 + 2 + 2)) = 500 kbps */
    LPC_CAN1->BTR   = 0x001C0009U;
    LPC_CAN1->MOD   = 0x00U;     /* Normal Operating Mode */
}

void CAN1_TxByte(uint32_t std_id, uint8_t data) {
    while ((LPC_CAN1->SR & (1U << 2)) == 0U); /* Wait TBS1 */
    LPC_CAN1->TFI1 = (1U << 16);              /* Standard 11-bit ID, DLC = 1 */
    LPC_CAN1->TID1 = std_id & 0x7FFU;
    LPC_CAN1->TDA1 = data;
    LPC_CAN1->CMR  = 0x21U;                   /* Transmit Buffer 1 */
}

uint8_t CAN1_RxAvailable(uint32_t *out_id, uint8_t *out_data) {
    if ((LPC_CAN1->SR & (1U << 0)) != 0U) {   /* RBS bit set */
        *out_id   = LPC_CAN1->RID & 0x7FFU;
        *out_data = (uint8_t)(LPC_CAN1->RDA & 0xFFU);
        LPC_CAN1->CMR = (1U << 2);            /* Release Receive Buffer (RRB) */
        return 1U;
    }
    return 0U;
}

int main(void) {
    uint32_t rx_id;
    uint8_t rx_byte;
    volatile uint32_t d;

    SystemInit();
    CAN1_Init();

    while (1) {
        CAN1_TxByte(0x120U, 0x55U);
        if (CAN1_RxAvailable(&rx_id, &rx_byte)) {
            /* Echo received byte back on ID + 1 */
            CAN1_TxByte(rx_id + 1U, rx_byte);
        }
        for (d = 0; d < 200000U; d++);
    }
}