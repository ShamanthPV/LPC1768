# NXP LPC1768 (ARM Cortex-M3) Bare-Metal

Register-level Embedded C implementations for the NXP LPC1768 microcontroller, developed and verified in **Keil µVision** and flashed via **Flash Magic (UART0 ISP)**.

## Toolchain & Hardware
- MCU: NXP LPC1768 (ARM Cortex-M3, 100 MHz CCLK / 25 MHz PCLK)
- IDE / Compiler: Keil µVision (ARM Compiler 5 / C90)
- Flashing Tool: Flash Magic (UART0 ISP @ 9600 bps, 12 MHz XTAL)

## Covered Modules
- 1.**GPIO:** LED patterns (Up/Down, Ring, Johnson counters), polling & debounced toggle inputs. 
- 2.**2x16 LCD:** 8-bit and 4-bit parallel interfacing using pointer-based drivers. 
- 3.**ADC**: 12-bit polling/threshold control and LM35 temperature sensing. 
- 4.**Timers:** Precision hardware delays (`TIM0`) and HC-SR04 ultrasonic distance measurement. 
- 5.**DAC:** 10-bit waveform synthesis (Square, Triangle, Sine lookup table) on `AOUT`. 
- 6.**PWM:** Duty-cycle modulation (`PWM1.1`) for LED dimming and H-bridge DC motor control. 
- 7.**I2C:** Master communication with PCF8574 I2C LCD and ADXL345 3-axis accelerometer. 
- 8.**UART:** Polling and interrupt-driven serial comms, EM-18 RFID access, and GSM AT commands. 
- 9.**CAN:** 500 kbps CAN1 node transmission, reception, and multi-node ID filtering. 
