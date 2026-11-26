#include <LPC17xx.h>
#include <stdint.h>

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

void LCD4_SendNibble(uint8_t nibble) {
    LPC_GPIO0->FIOCLR = LCD_NIBBLE_MASK;
    LPC_GPIO0->FIOSET = ((uint32_t)(nibble & 0x0FU)) << 4;
    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN;
    delay_ms(1);
}

void LCD4_Write(uint8_t byte, uint8_t is_data) {
    if (is_data) LPC_GPIO0->FIOSET = LCD_RS;
    else         LPC_GPIO0->FIOCLR = LCD_RS;

    LCD4_SendNibble(byte >> 4);   /* High nibble first */
    LCD4_SendNibble(byte & 0x0F); /* Low nibble second */
}

void LCD4_Init(void) {
    LPC_PINCON->PINSEL0 &= ~(0xFFU << 8);
    LPC_PINCON->PINSEL1 &= ~((3U << 6) | (3U << 8));
    LPC_GPIO0->FIODIR   |= (LCD_NIBBLE_MASK | LCD_RS | LCD_EN);

    delay_ms(20);
    LPC_GPIO0->FIOCLR = LCD_RS;
    LCD4_SendNibble(0x03); delay_ms(5);
    LCD4_SendNibble(0x03); delay_ms(2);
    LCD4_SendNibble(0x03);
    LCD4_SendNibble(0x02); /* Switch to 4-bit interface */

    LCD4_Write(0x28, 0); /* 4-bit, 2-line, 5x7 */
    LCD4_Write(0x0C, 0); /* Display ON, Cursor OFF */
    LCD4_Write(0x06, 0); /* Increment cursor */
    LCD4_Write(0x01, 0); /* Clear */
    delay_ms(5);
}

void LCD4_PrintPtr(const char *ptr) {
    while (*ptr != '\0') {
        LCD4_Write((uint8_t)(*ptr), 1);
        ptr++;
    }
}

int main(void) {
    SystemInit();
    LCD4_Init();

    LCD4_Write(0x80, 0);
    LCD4_PrintPtr("4-BIT LCD MODE");
    LCD4_Write(0xC0, 0);
    LCD4_PrintPtr("USING POINTERS");

    while (1);
}