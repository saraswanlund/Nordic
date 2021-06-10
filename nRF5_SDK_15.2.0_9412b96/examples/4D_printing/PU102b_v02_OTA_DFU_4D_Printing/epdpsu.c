#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "epdpsu.h"
#include "epdc.h"
#include "pl_i2c.h"			
#include "pmic-tps65185.h"

#include "platform.h"
#include "main.h"

/* --- GPIO --- */
extern struct tps65185_info pmic_info;

static int pl_epdpsu_gpio_on(struct pl_epdpsu *psu)
{
	struct pl_epdpsu_gpio *p = psu->data;
	unsigned timeout;
	uint8_t pgstat;		// for debugging

	if (psu->state)
		return 0;

	/* set the bit that enables the HV, and wait to see if it comes up */
	//pl_gpio_set(p->gpio, p->hv_en, 1);
        nrf_gpio_pin_write(PL_HVEN, 1);            // turn pin 15, HVEN on

//	pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x10, &pgstat);
//	printf("REVID: 0x%02X \t",pgstat);
//
//	pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x0F, &pgstat);
//	printf("PGSTAT: 0x%02X \t",pgstat);
//
//	pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x07, &pgstat);
//	printf("INTEN1: 0x%02X \t",pgstat);
//
//	pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x08, &pgstat);
//	printf("INTEN2: 0x%02X \n",pgstat);

	for (timeout = p->timeout_ms; timeout; timeout--) {
            pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x0F, &pgstat);
            printf("PGSTAT: 0x%02X \t",pgstat);

            pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x07, &pgstat);
            printf("INT1: 0x%02X \t",pgstat);

            pl_i2c_reg_read_8(pmic_info.i2c, (uint8_t) 0x68, (uint8_t) 0x08, &pgstat);
            printf("INT2: 0x%02X \n",pgstat);

            if (nrf_gpio_pin_read(PL_POK))      // read from pin 14
                break;
            nrf_delay_ms(1);
	}

	if (!timeout) {
            printf("POK timeout\n");
            //pl_gpio_set(p->gpio, p->hv_en, 0);
            nrf_gpio_pin_write(PL_HVEN, 0);            // turn pin 15, HVEN off

            return -1;
	}

	//pl_gpio_set(p->gpio, p->com_close, 1);
        nrf_gpio_pin_write(PL_HV_SW, 1);            // turn pin 17, HVSW on
	//msleep(p->on_delay_ms);
        nrf_delay_ms(300);
	psu->state = 1;

	return 0;
}

static int pl_epdpsu_gpio_off(struct pl_epdpsu *psu)
{
	struct pl_epdpsu_gpio *p = psu->data;

#if LOG_VERBOSE
	LOG("off");
#endif
        printf("Power Off\n");
	//pl_gpio_set(p->gpio, p->com_close, 0);
        nrf_gpio_pin_write(PL_HV_SW, 0);            // turn pin 17, HVSW on
	//pl_gpio_set(p->gpio, p->hv_en, 0);
        nrf_gpio_pin_write(PL_HVEN, 0);            // turn pin 15, HVEN off
	//msleep(p->off_delay_ms);
        nrf_delay_ms(300);
	psu->state = 0;
        
	return 0;
}

int pl_epdpsu_gpio_init(struct pl_epdpsu *psu, struct pl_epdpsu_gpio *p)
{
//	assert(psu != NULL);
//	assert(p != NULL);
//	assert(p->gpio != NULL);
//	assert(p->timeout_ms != 0);
//	assert(p->hv_en != PL_GPIO_NONE);
//	assert(p->com_close != PL_GPIO_NONE);
//	assert(p->pok != PL_GPIO_NONE);
//	assert(p->flt != PL_GPIO_NONE);

	psu->on = pl_epdpsu_gpio_on;
	psu->off = pl_epdpsu_gpio_off;
	psu->state = 0;
	psu->data = p;

	return 0;
}

/* --- EPDC --- */

static int pl_epdpsu_epdc_on(struct pl_epdpsu *psu)
{
	struct pl_epdc *epdc = psu->data;

	if (!psu->state) {
		if (epdc->set_epd_power(epdc, 1))
			return -1;

		psu->state = 1;
	}

	return 0;
}

static int pl_epdpsu_epdc_off(struct pl_epdpsu *psu)
{
	struct pl_epdc *epdc = psu->data;

	if (psu->state) {
		if (epdc->set_epd_power(epdc, 0))
			return -1;

		psu->state = 0;
	}

	return 0;
}

int pl_epdpsu_epdc_init(struct pl_epdpsu *psu, struct pl_epdc *epdc)
{
	psu->on = pl_epdpsu_epdc_on;
	psu->off = pl_epdpsu_epdc_off;
	psu->state = 0;
	psu->data = epdc;

	return 0;
}