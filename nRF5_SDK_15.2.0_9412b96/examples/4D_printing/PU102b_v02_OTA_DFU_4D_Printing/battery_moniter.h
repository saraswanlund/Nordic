/*
  Description: This file is for the BQ2744-G1A battery moniter. //note to self. Change BOM to the A model
*/

#ifndef INCLUDE_BATTERY_MONITER_H
#define INCLUDE_BATTERY_MONITER_H 1

# include "stdint.h"

#define BAT_MONITER_ADDR 0xAA


extern void batt_mon_init(void);

extern uint16_t batt_mon_read_voltage(void); //units mv (0 - 6000)

/*
extern uint16_t batt_mon_read_temp(void); //Units .1deg Kelvin
extern uint8_t batt_mon_read_SOC(void); //Units in %
extern uint16_t batt_mon_read_rem_cap(void); // units mAh
extern uint16_t batt_mon_read_control_status(void);
*/



#endif