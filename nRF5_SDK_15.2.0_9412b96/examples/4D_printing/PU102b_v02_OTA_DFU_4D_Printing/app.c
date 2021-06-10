/*
 * app/app.c -- Application
 *
 * Authors:
 *   
 *
 */
#include "platform.h"
#include "pl_endian.h"
#include "app.h"
#include "app_util.h" //Used for critical regions

#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "pl_i2c.h"   // debugging
#include "pmic-tps65185.h"

#include "eeprom-i2c.h"

//#define LOG_TAG "app"
//#include "utils.h"

int image_receivingG = 0;
int image_completeG = 0;

int pixel_x = 0;
int pixel_y = 0;

char RX_data[176];
int data_index;
unsigned char Image_bytes[150];

int app_demo(struct pl_platform *plat, uint16_t update, uint16_t display)
{
	if(update == 0){
//		app_update_pixels(plat);
	}
	else if(update == 1){
		app_update_display(plat);
	}
	return 0;
}

int app_clear(struct pl_platform *plat)
{
	struct pl_epdpsu *psu = &plat->psu;
	struct pl_epdc *epdc = &plat->epdc;
        uint8_t pgstat;		// for debugging
        struct tps65185_info pmic_info;

	if (epdc->fill(epdc, NULL, PL_BLACK))
		return -1;

        if (epdc->clear_init(epdc))
		return -1;

        if (psu->on(psu))
		return -1;

	if (epdc->update(epdc, pl_epdc_get_wfid(epdc, wf_init), NULL))
		return -1;

        if (epdc->wait_update_end(epdc))
		return -1;

        if (epdc->fill(epdc, NULL, PL_WHITE))
		return -1;

        if(psu->off(psu))
                return -1;

	return 0;
}

int app_fill_pixel(struct pl_platform *plat, unsigned int packet_num, unsigned char image_data){
	struct pl_epdc *epdc = &plat->epdc;

	if (epdc->fill_pixel(epdc, packet_num, image_data))
		return -1;

	return 0;
}

int app_load_image(struct pl_platform *plat, const struct pl_area *area, uint8_t data[]){
	struct pl_epdc *epdc = &plat->epdc;

	if (epdc->load_app_image(epdc, area, data))
		return -1;

	return 0;
}

// for testing filling an individual pixel
int app_manual_pixels(struct pl_platform *plat)
{
	struct pl_epdc *epdc = &plat->epdc;

	int wfid; /* = pl_epdc_get_wfid(epdc, wf_refresh);*/
//	int pixel_x = 100;
//	int pixel_y = 100;
//	int pixel_val = 0;

        printf("Manually Filling Pixels\n");

	wfid = pl_epdc_get_wfid(epdc, wf_refresh);

	if (wfid < 0)
		return -1;

	// fill_pixel function has been changed...
	// fill_pixel (epdc, gray level) - needs no x, y coordinates
//	for(pixel_y = 1; pixel_y < 21; pixel_y++){
//		for(pixel_x = 1; pixel_x < 21; pixel_x++){
//			epdc->fill_pixel_xy(epdc, PL_BLACK, pixel_x, pixel_y);
//			printf("Manually Filling Pixels x: %d \t y: %d\n", pixel_x, pixel_y);
//		}
//	}

	if (epdc->fill_pixel(epdc, 0, 0x88))
		return -1;
	if (epdc->fill_pixel(epdc, 1, 0x88))
		return -1;

	return 0;
}

int app_update_display(struct pl_platform *plat)
{
	struct pl_epdpsu *psu = &plat->psu;
	struct pl_epdc *epdc = &plat->epdc;

        CRITICAL_REGION_ENTER();

	int wfid; /* = pl_epdc_get_wfid(epdc, wf_refresh);*/
  
	wfid = pl_epdc_get_wfid(epdc, wf_refresh);

	printf("Updating display\n");

	if (wfid < 0)
		return -1;

	if (epdc->update_temp(epdc))
		return -1;

	if (psu->on(psu))
		return -1;

	if (epdc->update(epdc, wfid, NULL))
		return -1;

	if (epdc->wait_update_end(epdc))
		return -1;

	if (psu->off(psu))
		return -1;
        
        CRITICAL_REGION_EXIT();
        
	return 0;
}

int app_update_display_EEPROM(struct pl_platform *plat, struct i2c_eeprom *eeprom, uint8_t pic_num)
{
	struct pl_epdpsu *psu = &plat->psu;
	struct pl_epdc *epdc = &plat->epdc;

	int wfid; /* = pl_epdc_get_wfid(epdc, wf_refresh);*/
	wfid = pl_epdc_get_wfid(epdc, wf_refresh); 

        uint32_t offset = 0;
        uint32_t count = 0;
        uint32_t pixel_location = 0;
        uint8_t pixel_data = 0xFF;

        printf("Reading Pixels from EEPROM\n");

        if(pic_num == 1){
            printf("Reading 1st Pic : 0 ~ 96000\n");
            offset = 0;
        } else if(pic_num == 2){
            printf("Reading 2nd Pic : 96000 ~ 192000");
            offset = 96000;
        } else{
            printf("Wrong Picture Number");
            return -1;
        }
        
        pixel_location = 0;

        for(count = 0; count < 2000; count++){
            eeprom_read(eeprom, offset, 1, (uint8_t *) &pixel_data);
            
            if (epdc->fill_pixel(epdc, pixel_location, pixel_data))
		return -1;

            offset++;
            pixel_location++;
        }

	printf("Updating display\n");

	if (wfid < 0)
		return -1;

	if (epdc->update_temp(epdc))
		return -1;

	if (psu->on(psu))
		return -1;

	if (epdc->update(epdc, wfid, NULL))
		return -1;

	if (epdc->wait_update_end(epdc))
		return -1;

	if (psu->off(psu))
		return -1;

	return 0;
}

int app_pattern_check(struct pl_platform *plat)
{
	struct pl_epdc *epdc = &plat->epdc;
	struct pl_epdpsu *psu = &plat->psu;
	int wfid; /* = pl_epdc_get_wfid(epdc, wf_refresh);*/

	wfid = pl_epdc_get_wfid(epdc, wf_refresh);

	if (wfid < 0)
		return -1;

	if (epdc->pattern_check(epdc, 4))		// using a pattern_size variable from main.c - 5.17.2017
		return -1;

	if (epdc->update_temp(epdc))
		return -1;

	if (psu->on(psu))
		return -1;

	if (epdc->update(epdc, wfid, NULL))
		return -1;

	if (epdc->wait_update_end(epdc))
		return -1;

	if (psu->off(psu))
		return -1;

	return 0;
}

int app_pattern_vert_line(struct pl_platform *plat)
{
	struct pl_epdc *epdc = &plat->epdc;
	struct pl_epdpsu *psu = &plat->psu;
	int wfid; /* = pl_epdc_get_wfid(epdc, wf_refresh);*/

        printf("Refresh\n");
	wfid = pl_epdc_get_wfid(epdc, wf_refresh);

	if (wfid < 0)
		return -1;
        printf("Pattern\n");
	if (epdc->vert_line_pattern(epdc, 4))			// using a pattern_size variable from main.c - 5.17.2017
		return -1;
        printf("Update Temp\n");
	if (epdc->update_temp(epdc))
		return -1;

	if (psu->on(psu))
		return -1;
        printf("PSU ON\n");
	if (epdc->update(epdc, wfid, NULL))
		return -1;
        printf("Wait_end\n");
	if (epdc->wait_update_end(epdc))
		return -1;
        printf("OFF PSU\n");
	if (psu->off(psu))
		return -1;

	return 0;
}

int app_pattern_horiz_line(struct pl_platform *plat)
{
	struct pl_epdc *epdc = &plat->epdc;
	struct pl_epdpsu *psu = &plat->psu;
	int wfid; /* = pl_epdc_get_wfid(epdc, wf_refresh);*/

	wfid = pl_epdc_get_wfid(epdc, wf_refresh);

	if (wfid < 0)
		return -1;

	if (epdc->horiz_line_pattern(epdc, 4))			// using a pattern_size variable from main.c - 5.17.2017
		return -1;

	if (epdc->update_temp(epdc))
		return -1;

	if (psu->on(psu))
		return -1;

	if (epdc->update(epdc, wfid, NULL))
		return -1;

	if (epdc->wait_update_end(epdc))
		return -1;

	if (psu->off(psu))
		return -1;

	return 0;
}

int convert_binary_graylevel(unsigned char RX_data, unsigned int i)
{
	unsigned char binary;

	if(i == 0){
		binary = (RX_data & 0x01);
	}
	else if(i == 1){
		binary = (RX_data & 0x02) >> 1;
	}
	else if(i == 2){
		binary = (RX_data & 0x04) >> 2;
	}
	else if(i == 3){
		binary = (RX_data & 0x08) >> 3;
	}
	else if(i == 4){
		binary = (RX_data & 0x10) >> 4;
	}
	else if(i == 5){
		binary = (RX_data & 0x20) >> 5;
	}
	else if(i == 6){
		binary = (RX_data & 0x40) >> 6;
	}
	else if(i == 7){
		binary = (RX_data & 0x80) >> 7;
	}

	//printf("\nRX_data: %02x \t bit number, i: %d \t binary: %02x", RX_data, i, binary);

	if(binary == 0x00)
		return PL_WHITE;
	else
		return PL_BLACK;
}
