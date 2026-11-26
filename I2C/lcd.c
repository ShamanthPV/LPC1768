#include <LPC17xx.h>
#include <stdint.h>

#define PCF8574_ADDR (0x27U << 1)
#define LCD_BL       0x08U
#define LCD_EN       0x04U
#define LCD_RS       0x01U

void delay_ms(uint32_t ms) {
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 20000; j++);
    }
}

void I2C0_Init(void) {
    LPC_SC->PCONP |= (1U << 7); /* PCI2C0 */
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(0x0FU << 22)) | (1U << 22) | (1U << 24);
    LPC_I2C0->I2SCLH = 125U; /* 25 MHz / (125 + 125) = 100 kHz */
    LPC_I2C0->I2SCLL = 125U;
    LPC_I2C0->I2CONCLR = 0x6CU;
    LPC_I2C0->I2CONSET = (1U << 6); /* I2EN */
}

void I2C0_WriteByte(uint8_t sla_w, uint8_t data) {
    LPC_I2C0->I2CONSET = (1U << 5); /* STA */
    while ((LPC_I2C0->I2CONSET & (1U << 3)) == 0U); /* Wait SI */
    LPC_I2C0->I2CONCLR = (1U << 5);

    LPC_I2C0->I2DAT = sla_w;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while ((LPC_I2C0->I2CONSET & (1U << 3)) == 0U);

    LPC_I2C0->I2DAT = data;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while ((LPC_I2C0->I2CONSET & (1U << 3)) == 0U);

    LPC_I2C0->I2CONSET = (1U << 4); /* STO */
    LPC_I2C0->I2CONCLR = (1U << 3);
}

void I2C_LCD_Nibble(uint8_t nib, uint8_t rs) {
    uint8_t pkt = (uint8_t)((nib & 0xF0U) | LCD_BL | (rs ? LCD_RS : 0U));
    I2C0_WriteByte(PCF8574_ADDR, pkt | LCD_EN);
    delay_ms(1);
    I2C0_WriteByte(PCF8574_ADDR, pkt);
}

void I2C_LCD_Write(uint8_t b, uint8_t rs) {
    I2C_LCD_Nibble(b & 0xF0U, rs);
    I2C_LCD_Nibble((uint8_t)(b << 4), rs);
}

int main(void) {
    const char *msg = "I2C LCD ONLINE";

    SystemInit();
    I2C0_Init();
    delay_ms(50);

    I2C_LCD_Nibble(0x30, 0); delay_ms(5);
    I2C_LCD_Nibble(0x30, 0);
    I2C_LCD_Nibble(0x20, 0);
    I2C_LCD_Write(0x28, 0);
    I2C_LCD_Write(0x0C, 0);
    I2C_LCD_Write(0x01, 0);
    delay_ms(5);

    while (*msg) I2C_LCD_Write((uint8_t)(*msg++), 1);
    while (1);
}