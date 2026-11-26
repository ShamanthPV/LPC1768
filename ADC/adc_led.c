#include <LPC17xx.h>
#include <stdint.h>

#define LED_PIN (1U << 25)

void ADC_Init(void) {
    LPC_SC->PCONP |= (1U << 12);          /* Power ON ADC (PCADC) */
    LPC_PINCON->PINSEL1 &= ~(3U << 14);
    LPC_PINCON->PINSEL1 |= (1U << 14);    /* P0.23 -> AD0.0 */
    /* Select AD0.0 (bit 0), CLKDIV = 4 (25MHz/5 = 5MHz), PDN = 1 (bit 21) */
    LPC_ADC->ADCR = (1U << 0) | (4U << 8) | (1U << 21);
}

uint16_t ADC_ReadCh0(void) {
    uint32_t result;
    LPC_ADC->ADCR |= (1U << 24); /* Start conversion now */
    do {
        result = LPC_ADC->ADDR0;
    } while ((result & (1U << 31)) == 0U); /* Wait for DONE bit */
    return (uint16_t)((result >> 4) & 0x0FFFU);
}

int main(void) {
    uint16_t adc_val;

    SystemInit();
    ADC_Init();
    LPC_PINCON->PINSEL1 &= ~(3U << 18);
    LPC_GPIO0->FIODIR |= LED_PIN;

    while (1) {
        adc_val = ADC_ReadCh0();
        if (adc_val > 2048U) { /* > 1.65V threshold */
            LPC_GPIO0->FIOSET = LED_PIN;
        } else {
            LPC_GPIO0->FIOCLR = LED_PIN;
        }
    }
}