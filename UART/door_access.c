#include <LPC17xx.h>
#include <stdint.h>
#include <string.h>

#define DOOR_PIN (1U << 25)

void UART1_Init(void) {
    LPC_SC->PCONP |= (1U << 4); /* PCUART1 */
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(3U << 30)) | (1U << 30);
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3U << 0))  | (1U << 0);

    LPC_UART1->LCR = 0x83U;
    LPC_UART1->DLL = 163U; /* 9600 bps */
    LPC_UART1->DLM = 0U;
    LPC_UART1->LCR = 0x03U;
    LPC_UART1->FCR = 0x07U;
}

char UART1_RxChar(void) {
    while ((LPC_UART1->LSR & (1U << 0)) == 0U);
    return (char)LPC_UART1->RBR;
}

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

int main(void) {
    const char AUTH_TAG[13] = "180088F2A1C3"; /* Replace with your 12-char tag ID */
    char tag_buf[13];
    uint8_t idx;

    SystemInit();
    UART1_Init();
    LPC_PINCON->PINSEL1 &= ~(3U << 18);
    LPC_GPIO0->FIODIR |= DOOR_PIN;

    while (1) {
        for (idx = 0U; idx < 12U; idx++) {
            tag_buf[idx] = UART1_RxChar();
        }
        tag_buf[12] = '\0';

        if (strncmp(tag_buf, AUTH_TAG, 12U) == 0) {
            LPC_GPIO0->FIOSET = DOOR_PIN; /* Unlock door */
            delay_ms(3000U);
            LPC_GPIO0->FIOCLR = DOOR_PIN;
        }
    }
}