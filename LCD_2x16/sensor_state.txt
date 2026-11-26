#include <LPC17xx.h>
#include <stdint.h>

#define LCD_DATA_MASK (0xFFU << 4)
#define LCD_RS        (1U << 19)
#define LCD_EN        (1U << 20)
#define SENSOR_PIN    (1U << 24)

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

void LCD8_Cmd(uint8_t cmd) {
    LPC_GPIO0->FIOCLR = LCD_RS | LCD_DATA_MASK;
    LPC_GPIO0->FIOSET = ((uint32_t)cmd) << 4;
    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(2);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

void LCD8_Data(uint8_t dat) {
    LPC_GPIO0->FIOSET = LCD_RS;
    LPC_GPIO0->FIOCLR = LCD_DATA_MASK;
    LPC_GPIO0->FIOSET = ((uint32_t)dat) << 4;
    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(2);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

void LCD8_Print(const char *s) {
    while (*s) LCD8_Data((uint8_t)(*s++));
}

int main(void) {
    SystemInit();
    LPC_PINCON->PINSEL0 &= ~(0xFFFFU << 8);
    LPC_PINCON->PINSEL1 &= ~((3U << 6) | (3U << 8) | (3U << 16));
    LPC_PINCON->PINMODE1 &= ~(3U << 16);

    LPC_GPIO0->FIODIR |= (LCD_DATA_MASK | LCD_RS | LCD_EN);
    LPC_GPIO0->FIODIR &= ~SENSOR_PIN;

    delay_ms(20);
    LCD8_Cmd(0x38);
    LCD8_Cmd(0x0C);
    LCD8_Cmd(0x01);
    delay_ms(5);

    LCD8_Cmd(0x80);
    LCD8_Print("SENSOR MONITOR");

    while (1) {
        LCD8_Cmd(0xC0);
        if ((LPC_GPIO0->FIOPIN & SENSOR_PIN) == 0U) {
            LCD8_Print("STATUS: DETECTED");
        } else {
            LCD8_Print("STATUS: CLEAR   ");
        }
        delay_ms(100);
    }
}