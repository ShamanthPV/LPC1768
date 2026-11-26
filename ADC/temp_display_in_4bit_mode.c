#include <LPC17xx.h>
#include <stdint.h>
#include <stdio.h>

#define LCD_NIBBLE_MASK (0x0FU << 4)
#define LCD_RS          (1U << 19)
#define LCD_EN          (1U << 20)

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

void LCD4_Nibble(uint8_t n) {
    LPC_GPIO0->FIOCLR = LCD_NIBBLE_MASK;
    LPC_GPIO0->FIOSET = ((uint32_t)(n & 0x0FU)) << 4;
    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN;
    delay_ms(1);
}

void LCD4_Write(uint8_t b, uint8_t is_data) {
    if (is_data) LPC_GPIO0->FIOSET = LCD_RS;
    else         LPC_GPIO0->FIOCLR = LCD_RS;
    LCD4_Nibble(b >> 4);
    LCD4_Nibble(b & 0x0F);
}

void LCD4_Init(void) {
    LPC_PINCON->PINSEL0 &= ~(0xFFU << 8);
    LPC_PINCON->PINSEL1 &= ~((3U << 6) | (3U << 8));
    LPC_GPIO0->FIODIR   |= (LCD_NIBBLE_MASK | LCD_RS | LCD_EN);
    delay_ms(20);
    LPC_GPIO0->FIOCLR = LCD_RS;
    LCD4_Nibble(0x03); delay_ms(5);
    LCD4_Nibble(0x03); LCD4_Nibble(0x03); LCD4_Nibble(0x02);
    LCD4_Write(0x28, 0); LCD4_Write(0x0C, 0); LCD4_Write(0x01, 0);
    delay_ms(5);
}

void LCD4_Print(const char *s) {
    while (*s) LCD4_Write((uint8_t)(*s++), 1);
}

uint16_t ADC_ReadCh0(void) {
    uint32_t res;
    LPC_ADC->ADCR |= (1U << 24);
    do { res = LPC_ADC->ADDR0; } while ((res & (1U << 31)) == 0U);
    return (uint16_t)((res >> 4) & 0x0FFFU);
}

int main(void) {
    uint16_t raw;
    uint32_t temp_x10;
    char buf[17];

    SystemInit();
    LCD4_Init();

    LPC_SC->PCONP |= (1U << 12);
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3U << 14)) | (1U << 14);
    LPC_ADC->ADCR = (1U << 0) | (4U << 8) | (1U << 21);

    LCD4_Write(0x80, 0);
    LCD4_Print("LM35 TEMP SENSOR");

    while (1) {
        raw = ADC_ReadCh0();
        /* LM35 = 10 mV/C; Vref = 3300 mV -> Temp*10 = (raw * 3300) / 4095 */
        temp_x10 = ((uint32_t)raw * 3300U) / 4095U;
        sprintf(buf, "Temp: %2lu.%1lu C   ", temp_x10 / 10U, temp_x10 % 10U);
        LCD4_Write(0xC0, 0);
        LCD4_Print(buf);
        delay_ms(300);
    }
}