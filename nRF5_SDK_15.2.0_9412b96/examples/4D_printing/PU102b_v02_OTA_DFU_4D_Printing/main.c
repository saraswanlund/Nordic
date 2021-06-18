                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /**
 * Copyright (c) 2014 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"

#include "bsp_btn_ble.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#define CONFIG_NFCT_PINS_AS_GPIOS     1

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "bluetooth.h"
#include "pl_i2c.h"
#include "pl_spi.h"
#include "epson-s1d135xx.h"
#include "platform.h"
#include "hwinfo.h"
#include "dispinfo.h"
#include "eeprom-i2c.h"
#include "probe.h"
#include "vcom.h"
#include "pmic-tps65185.h"
#include "app.h"
#include "main.h"
#include "eeprom-i2c.h"
#include "nrfx_gpiote.h"

#include "nrf_sdm.h"

#include "sensor_als.h"
#include "sensor_accel.h"

#include "battery_moniter.h"

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"

// for ADC sampling - 6.18.2020 SJE
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

/* --- hardware info --- */
#define BOARD_MAJ 6
#define BOARD_MIN 3
#define CONFIG_DEFAULT_I2C_MODE I2C_MODE_HOST

uint8_t buffer1_full;
uint8_t buffer2_full;
uint8_t buffer_use;
uint32_t disp_buffer_flag = 0;
uint32_t packet_flagG = 0;
uint8_t stealth_pkt_flag = 0;
uint8_t BT_connectionG = 0;
uint8_t Alexa_TaskG = 0;
char commandG = 0;

uint8_t battery_state_last;
uint8_t buffer_chksm;

uint8_t adc1[2] = {0};
uint8_t adc2[2] = {0};

static const struct pl_hwinfo g_hwinfo_default = {
	/* version */
	PL_HWINFO_VERSION,
	/* vcom */
	{ 127, 4172, 381, 12490, 25080, -32300, 56886 },
	/* board */
	{ "HB", BOARD_MAJ, BOARD_MIN, 0, HV_PMIC_TPS65185, 0, 0, 0,
	  CONFIG_DEFAULT_I2C_MODE, TEMP_SENSOR_NONE, 0, EPDC_S1D13541, 1, 1 },
	/* CRC16 (not used when not reading from actual EEPROM) */
	0xFFFF,
};
#define HWINFO_DEFAULT (&g_hwinfo_default)

struct pl_dispinfo dispinfo;
struct tps65185_info pmic_info;
static struct pl_epdpsu_gpio g_epdpsu_gpio = {
	&g_plat.gpio, PL_HVEN, PL_HV_SW, PL_POK, PMIC_FLT, 300, 5, 100
};

static const struct s1d13541_data g_s1d13541_data = {
    EPSON_RESET, EPSON_CS, EPSON_HIRQ, PL_GPIO_NONE, EPSON_HDC, EPSON_CLK_EN, EPSON_VCC_EN
};

void Status_LED(uint8_t state);
void Blink_LED(uint16_t count, uint32_t time);

struct i2c_als als = {
    NULL, I2C_ALS_ADDR ,LTR_329ALS
};

struct i2c_accel accel = {
    NULL, I2C_ACCEL_ADDR ,LIS3DH
};

uint16_t accel_data[3];

// for ADC sampling - 6.18.2020 SJE
#define SAMPLES_IN_BUFFER 2

static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);
static nrf_saadc_value_t     m_buffer_pool[2][SAMPLES_IN_BUFFER];
static nrf_ppi_channel_t     m_ppi_channel;
static uint32_t              m_adc_evt_counter;
 
void timer_handler(nrf_timer_event_t event_type, void * p_context)
{

};


void saadc_sampling_event_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event every 400ms */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 1000);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}


void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);

    APP_ERROR_CHECK(err_code);
}


void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

        int i, buffer;
//        NRF_LOG_INFO("ADC event number: %d", (int)m_adc_evt_counter);


        for (i = 0; i < SAMPLES_IN_BUFFER; i++)
        {
            if(p_event->data.done.p_buffer[i] < 0){
                buffer = 0;
            }else{
                buffer = p_event->data.done.p_buffer[i];
            }

            if((i % 2) == 0){
                adc1[1] = (buffer & 0x00FF);
                adc1[0] = ((buffer & 0xFF00) >> 8);
                //send_adc1(&adc1);
                printf("Channel 1 = %d \t ", adc1[1] + (adc1[1] << 0));
            }else{
                adc2[1] = (buffer & 0x00FF);
                adc2[0] = ((buffer & 0xFF00) >> 8);
                //send_adc2(&adc2);
                printf("Channel 2 = %d\n", adc2[1] + (adc2[1] << 0));
            }
        }
        m_adc_evt_counter++;
    }
}


void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config1 =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);

    nrf_saadc_channel_config_t channel_config2 =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config1);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(1, &channel_config2);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

}

int main(void)
{
    log_init();
<<<<<<< HEAD
    printf("Hello World! \n");
   ret_code_t err_code;
    struct pl_i2c host_i2c;
    struct pl_i2c disp_i2c;
    struct pl_wflib_eeprom_ctx wflib_eeprom_ctx;
    struct pl_dispinfo dispinfo;
    struct vcom_cal vcom_cal;
    struct s1d13541 s1d13541 = { &g_s1d13541_data, &g_plat.gpio };
    struct pl_epdc *epdc = &g_plat.epdc;
    bool erase_bonds;

    struct pl_area image_area;

    struct i2c_eeprom disp_eeprom = {
            NULL, I2C_DISPINFO_EEPROM_ADDR, EEPROM_24AA256
    };
    struct i2c_eeprom hw_eeprom = {
        &host_i2c, I2C_HWINFO_EEPROM_ADDR, EEPROM_CAT24M01,
    };

=======
    printf("Hello World! log init done \n\n");
   
>>>>>>> experimental
    // Example code for MDBT42Q NFC module (nRF52832 chip)
    // configure Pin 25 as Output
    nrf_gpio_cfg(STATUS_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0H1,NRF_GPIO_PIN_NOSENSE);         
    printf("gpio configured \n\n");
    Status_LED(1);

    printf("SWK PU102 RevB - Software v02 with OTA DFU & Alexa \n");

<<<<<<< HEAD
    // Initialize the async SVCI interface to bootloader before any interrupts are enabled. - 5.12.2020 SJE
    printf("Initializing the async SVCI interface to bootloader\n");
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);
    if(err_code != NRF_SUCCESS){
        printf("Error in Configuring BLE DFU Service \n");
    }

    timers_init();
    err_code = nrfx_gpiote_init(); // Initialize pin interrupt driver
    APP_ERROR_CHECK(err_code);

    saadc_init();
    saadc_sampling_event_init();
    saadc_sampling_event_enable();
    printf("SAADC HAL simple example started.");

    //Initialize Bluetooth 
    power_management_init();
    ble_stack_init(); //this line is stopping the code
    gap_params_init();               
    gatt_init(); 
    services_init();  //this line is also stopping the code                   
    conn_params_init();                   
    peer_manager_init(); 
    advertising_init(false); //this line is also stopping the code
=======
    timers_init();
    printf("timers init done \n\n");
 
    //Initialize Bluetooth 
    power_management_init();
    printf("power management init done \n\n");
    ble_stack_init(); 
    gap_params_init();               
    gatt_init(); 
    printf("gatt init done \n\n");
    services_init();                     
    conn_params_init();                   
    peer_manager_init(); 
    printf("peer manager init \n\n");
    advertising_init(false); 
>>>>>>> experimental
    NVIC_SetPriority(SD_EVT_IRQn, 7);  //set lowest priority to BLE
    NRF_LOG_INFO("Bluetooth is Initialized");
    printf("Bluetooth is Initialized.\n");

    // Start Execution
    application_timers_start();
<<<<<<< HEAD
    advertising_start(false);            //this line is alse stopping the code.....true on erasing bonds - meaning deleting every previous bonding information on nRF chip
=======
    advertising_start(false);            //true on erasing bonds - meaning deleting every previous bonding information on nRF chip
>>>>>>> experimental
                                         
                                         // false on erasing bonds - nRF will not delete any previous bonding information

    Status_LED(0);
    // Enter main loop.
    while(1)
   {
      NRF_LOG_FLUSH();

      if(BT_connectionG == 1){
          Status_LED(1);
      } else if(BT_connectionG == 0){
          Status_LED(0);
      } else{
          Blink_LED(5,500);
      }

      if (NRF_LOG_PROCESS() == false){
          sd_app_evt_wait(); //Tells the processor to sleep until an event is triggered
      }
   }
}

void Status_LED(uint8_t state){
    nrf_gpio_pin_write(25, state);
}

void Blink_LED(uint16_t count, uint32_t time){
    uint8_t i = 0;
    while(1){
        for(i = 0; i < count; i++){
            Status_LED(1);
            nrf_delay_ms(time);
            Status_LED(0);
            nrf_delay_ms(time);
        }
        nrf_delay_ms(1500);
    }
}