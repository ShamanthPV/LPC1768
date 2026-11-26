#include <LPC17xx.h>
#include <stdint.h>

#define ADXL345_W (0x53U << 1)
#define ADXL345_R ((0x53U << 1) | 1U)
#define LED_PIN   (1U << 25)

void I2C0_Init(void) {
    LPC_SC->PCONP |= (1U << 7);
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(0x0FU << 22)) | (1U << 22) | (1U << 24);
    LPC_I2C0->I2SCLH = 125U;
    LPC_I2C0->I2SCLL = 125U;
    LPC_I2C0->I2CONCLR = 0x6CU;
    LPC_I2C0->I2CONSET = (1U << 6);
}

void I2C0_WriteReg(uint8_t reg, uint8_t val) {
    LPC_I2C0->I2CONSET = (1U << 5);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));
    LPC_I2C0->I2CONCLR = (1U << 5);

    LPC_I2C0->I2DAT = ADXL345_W;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));

    LPC_I2C0->I2DAT = reg;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));

    LPC_I2C0->I2DAT = val;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));

    LPC_I2C0->I2CONSET = (1U << 4);
    LPC_I2C0->I2CONCLR = (1U << 3);
}

uint8_t I2C0_ReadReg(uint8_t reg) {
    uint8_t val;
    LPC_I2C0->I2CONSET = (1U << 5);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));
    LPC_I2C0->I2CONCLR = (1U << 5);

    LPC_I2C0->I2DAT = ADXL345_W;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));

    LPC_I2C0->I2DAT = reg;
    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));

    LPC_I2C0->I2CONSET = (1U << 5); /* Repeated START */
    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));
    LPC_I2C0->I2CONCLR = (1U << 5);

    LPC_I2C0->I2DAT = ADXL345_R;
    LPC_I2C0->I2CONCLR = (1U << 3) | (1U << 2); /* Clear AA for NACK */
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));

    LPC_I2C0->I2CONCLR = (1U << 3);
    while (!(LPC_I2C0->I2CONSET & (1U << 3)));
    val = (uint8_t)LPC_I2C0->I2DAT;

    LPC_I2C0->I2CONSET = (1U << 4);
    LPC_I2C0->I2CONCLR = (1U << 3);
    return val;
}

int main(void) {
    int16_t x_axis;

    SystemInit();
    I2C0_Init();
    LPC_PINCON->PINSEL1 &= ~(3U << 18);
    LPC_GPIO0->FIODIR |= LED_PIN;

    I2C0_WriteReg(0x2DU, 0x08U); /* ADXL345 POWER_CTL: Measurement Mode */

    while (1) {
        x_axis = (int16_t)(((uint16_t)I2C0_ReadReg(0x33U) << 8) | I2C0_ReadReg(0x32U));
        if (x_axis > 120 || x_axis < -120) {
            LPC_GPIO0->FIOSET = LED_PIN;
        } else {
            LPC_GPIO0->FIOCLR = LED_PIN;
        }
    }
}