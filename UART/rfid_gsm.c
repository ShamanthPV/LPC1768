#include <LPC17xx.h>
#include <stdint.h>

void UART0_Init(void) {
    LPC_SC->PCONP |= (1U << 3); /* PCUART0 */
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(0x0FU << 4)) | (1U << 4) | (1U << 6);

    LPC_UART0->LCR = 0x83U; /* 8-bit, 1 stop, no parity, DLAB = 1 */
    LPC_UART0->DLL = 163U;  /* 25 MHz / (16 * 163) = 9586 bps (~9600) */
    LPC_UART0->DLM = 0U;
    LPC_UART0->LCR = 0x03U; /* DLAB = 0 */
    LPC_UART0->FCR = 0x07U; /* Enable & reset FIFOs */
}

void UART0_TxChar(char c) {
    while ((LPC_UART0->LSR & (1U << 5)) == 0U); /* Wait THRE */
    LPC_UART0->THR = (uint8_t)c;
}

void UART0_TxString(const char *s) {
    while (*s) UART0_TxChar(*s++);
}

char UART0_RxChar(void) {
    while ((LPC_UART0->LSR & (1U << 0)) == 0U); /* Wait RDR */
    return (char)LPC_UART0->RBR;
}

int main(void) {
    char ch;

    SystemInit();
    UART0_Init();
    UART0_TxString("LPC1768 UART0 Ready. Type characters:\r\n");

    while (1) {
        ch = UART0_RxChar();
        UART0_TxChar(ch);
    }
}