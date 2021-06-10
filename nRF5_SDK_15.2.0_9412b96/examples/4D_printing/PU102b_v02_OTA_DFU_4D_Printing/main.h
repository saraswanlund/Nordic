#ifndef MAIN_H
#define MAIN_H 1

#include "platform.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"

#define I2C_HWINFO_EEPROM_ADDR 0x50
#define I2C_DISPINFO_EEPROM_ADDR 0x54

#define EINK_PIXEL_CNT 96000

// Define All I/O Pins for Smart Tupperware
#define EPSON_RESET           6   //Reset pin for the Plastic Logic Panel
#define EPSON_CS              5   //Chip select pin for the Plastic Logic Panel
#define EPSON_HIRQ            8   //Interrupt Line for Plastic Logic Panel
#define EPSON_HDC             7
#define EPSON_CLK_EN          12  
#define EPSON_VCC_EN          11

#define PL_HVEN               14
#define PL_POK                13
#define PL_HV_SW              15
#define PMIC_FLT              NULL


#define STATUS_PIN            25

#define BAT_GAUGE_LOW         16
#define BAT_CHARGE_ST         17

#define ACCEL_INT1            30
#define ACCEL_INT2            31

#define I2C_HWINFO_EEPROM_ADDR 0x50
#define I2C_DISPINFO_EEPROM_ADDR 0x54
#define I2C_ALS_ADDR 0x29
#define I2C_ACCEL_ADDR 0x18 //address in schematic is wrong not 0x28


static struct pl_platform g_plat;  //Expose g_plat to everyone
static void gpiote_event_handler(nrfx_gpiote_pin_t  pin,  nrf_gpiote_polarity_t action);

extern uint32_t disp_buffer_flag;       // number count of 200bytes packets - every 24 of these = increment in packet_flag
extern uint32_t packet_flagG;           // number count of 4800bytes packets
extern uint8_t buffer1_full;            // indicate a full buffer1 that needs to be read
extern uint8_t buffer2_full;            // indicate a full buffer2 that needs to be read
extern uint8_t buffer_use;              // indicate which buffer is currently in use of filling from BLE

extern uint8_t BT_connectionG;
extern char commandG;
extern uint8_t stealth_flagG;
extern uint8_t stealth_timerG;
extern uint8_t Alexa_TaskG;

extern uint8_t adc1[2];
extern uint8_t adc2[2];

extern void Status_LED(uint8_t state);

void saadc_sampling_event_init(void);
void saadc_sampling_event_enable(void);

#endif