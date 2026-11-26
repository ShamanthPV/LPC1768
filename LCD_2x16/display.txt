#include <LPC17xx.h>
#include <stdint.h>

#define LCD_DATA_MASK (0xFFU << 4)
#define LCD_RS        (1U << 19)
#define LCD_EN        (1U << 20)

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

void LCD8_PulseEnable(void) {
    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(2);
    LPC_GPIO0->FIOCLR = LCD_EN;
    delay_ms(2);
}

void LCD8_WriteByte(uint8_t val, uint8_t is_data) {
    if (is_data) LPC_GPIO0->FIOSET = LCD_RS;
    else         LPC_GPIO0->FIOCLR = LCD_RS;

    LPC_GPIO0->FIOCLR = LCD_DATA_MASK;
    LPC_GPIO0->FIOSET = ((uint32_t)val) << 4;
    LCD8_PulseEnable();
}

void LCD8_Init(void) {
    LPC_PINCON->PINSEL0 &= ~(0xFFFFU << 8);          /* P0.4..P0.11 GPIO */
    LPC_PINCON->PINSEL1 &= ~((3U << 6) | (3U << 8)); /* P0.19, P0.20 GPIO */
    LPC_GPIO0->FIODIR   |= (LCD_DATA_MASK | LCD_RS | LCD_EN);

    delay_ms(20);
    LCD8_WriteByte(0x38, 0); /* 8-bit, 2 lines, 5x7 font */
    LCD8_WriteByte(0x0C, 0); /* Display ON, Cursor OFF */
    LCD8_WriteByte(0x06, 0); /* Auto-increment cursor */
    LCD8_WriteByte(0x01, 0); /* Clear display */
    delay_ms(5);
}

void LCD8_Print(const char *str) {
    while (*str) {
        LCD8_WriteByte((uint8_t)(*str++), 1);
    }
}

int main(void) {
    SystemInit();
    LCD8_Init();

    LCD8_WriteByte(0x80, 0); /* Line 1, Col 0 */
    LCD8_Print("LPC1768 8-BIT");
    LCD8_WriteByte(0xC0, 0); /* Line 2, Col 0 */
    LCD8_Print("LCD WORKING!");

    while (1);
}