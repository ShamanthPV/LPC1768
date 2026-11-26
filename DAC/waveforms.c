#include <LPC17xx.h>
#include <stdint.h>

/* 32-sample 10-bit Sine Lookup Table (0..1023) */
static const uint16_t SINE_LUT[32] = {
    512, 612, 708, 796, 874, 938, 985, 1014,
    1023, 1014, 985, 938, 874, 796, 708, 612,
    512, 412, 316, 228, 150, 86, 39, 10,
    0, 10, 39, 86, 150, 228, 316, 412
};

void DAC_Write(uint16_t val_10bit) {
    LPC_DAC->DACR = ((uint32_t)(val_10bit & 0x03FFU)) << 6;
}

void delay_cycles(uint32_t c) {
    volatile uint32_t i;
    for (i = 0; i < c; i++);
}

int main(void) {
    uint16_t k, val;

    SystemInit();
    /* Configure P0.26 as AOUT (PINSEL1 bits [21:20] = 10) */
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3U << 20)) | (2U << 20);

    while (1) {
        /* 1. Square Wave (10 cycles) */
        for (k = 0; k < 10U; k++) {
            DAC_Write(1023U); delay_cycles(5000U);
            DAC_Write(0U);    delay_cycles(5000U);
        }

        /* 2. Triangle Wave (5 cycles) */
        for (k = 0; k < 5U; k++) {
            for (val = 0U; val < 1023U; val += 8U) {
                DAC_Write(val); delay_cycles(40U);
            }
            for (val = 1023U; val > 8U; val -= 8U) {
                DAC_Write(val); delay_cycles(40U);
            }
        }

        /* 3. Sine Wave (50 cycles - Sl.No 27) */
        for (k = 0; k < 50U; k++) {
            uint8_t idx;
            for (idx = 0U; idx < 32U; idx++) {
                DAC_Write(SINE_LUT[idx]);
                delay_cycles(150U);
            }
        }
    }
}