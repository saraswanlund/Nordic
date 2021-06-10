#ifndef INCLUDE_PL_EPDPSU_H
#define INCLUDE_PL_EPDPSU_H 1



/**
   @file pl/epdpsu.h

   Abstract interface and generic implementation to the EPD PSU
*/

struct pl_epdc;

/** Interface */
struct pl_epdpsu {
	/**
	   turn the EPD PSU on
	   @param[in] psu pl_epdpsu instance
	   @return -1 if an error occured, 0 otherwise
	 */
	int (*on)(struct pl_epdpsu *psu);

	/**
	   turn the EPD PSU off
	   @param[in] psu pl_epdpsu instance
	   @return -1 if an error occured, 0 otherwise
	 */
	int (*off)(struct pl_epdpsu *psu);

	int state;            /**< current power state (1=on, 0=off) */
	void *data;           /**< private data for the implementation */
};

/** Generic GPIO-based implementation */
struct pl_epdpsu_gpio {
	struct pl_gpio *gpio; /**< pl_gpio instance to control the GPIOs */
	unsigned hv_en;       /**< GPIO number to turn the power on/off */
	unsigned com_close;   /**< GPIO number to close the COM switch */
	unsigned pok;         /**< GPIO number to read Power OK */
	unsigned flt;         /**< GPIO number to read Power FLT status */
	unsigned timeout_ms;  /**< Maximum time in ms to wait for POK */
	unsigned on_delay_ms; /**< Delay after turning the power on */
	unsigned off_delay_ms;/**< Delay after turning the power off */
};

/**
   Initialise a pl_epdpsu instance with generic GPIO-based implamentation.

   Both the pl_epdpsu and pl_epdpsu_gpio structures need to be managed by the
   caller, so they can be either on the heap or the caller's stack.

   @param[in] psu pl_epdpsu instance
   @param[in] p pl_epdpsu_gpio instance
   @return -1 if an error occured, 0 otherwise
*/
extern int pl_epdpsu_gpio_init(struct pl_epdpsu *psu, struct pl_epdpsu_gpio *p);

/**
   Initialise a pl_epdpsu instance to use the generic epdc->set_epd_power

   @param[in] psu pl_epdpsu instance
   @param[in] epdc pl_epdc instance
   @return -1 if an error occured, 0 otherwise
 */
extern int pl_epdpsu_epdc_init(struct pl_epdpsu *psu, struct pl_epdc *epdc);

#endif /* INCLUDE_PL_EPDPSU_H */
