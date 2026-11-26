#include <LPC17xx.h>
#include <stdint.h>

void UART1_Init(void) {
    LPC_SC->PCONP |= (1U << 4);
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(3U << 30)) | (1U << 30);
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3U << 0))  | (1U << 0);
    LPC_UART1->LCR = 0x83U;
    LPC_UART1->DLL = 163U;
    LPC_UART1->DLM = 0U;
    LPC_UART1->LCR = 0x03U;
    LPC_UART1->FCR = 0x07U;
}

void UART1_TxChar(char c) {
    while ((LPC_UART1->LSR & (1U << 5)) == 0U);
    LPC_UART1->THR = (uint8_t)c;
}

void UART1_TxStr(const char *s) {
    while (*s) UART1_TxChar(*s++);
}

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

int main(void) {
    SystemInit();
    UART1_Init();
    delay_ms(2000U);

    UART1_TxStr("AT\r\n");
    delay_ms(1000U);
    UART1_TxStr("AT+CMGF=1\r\n"); /* Text SMS mode */
    delay_ms(1000U);
    UART1_TxStr("AT+CMGS=\"+919999999999\"\r\n");
    delay_ms(1000U);
    UART1_TxStr("LPC1768 GSM Alert Triggered");
    UART1_TxChar(0x1A); /* Ctrl+Z to transmit SMS */

    while (1);
}