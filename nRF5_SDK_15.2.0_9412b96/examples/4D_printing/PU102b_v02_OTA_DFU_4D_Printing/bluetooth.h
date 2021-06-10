#ifndef BLUETOOTH_H__
#define BLUETOOTH_H__

#define DEVICE_NAME                     "SWK"                       /**< Name of device. Will be included in the advertising data. */                     
#define MANUFACTURER_NAME               "P University"                          /**< Manufacturer. Will be passed to Device Information Service. */
#define GAP_APPEARANCE                  100                                     /** The appearance number corresponds with a device type, e.g. heart rate monitor. 
                                                                                    There are none assigned to Tupperware
                                                                                    ;therefore, assigned an unassigned number from the SIG */
#define APP_ADV_INTERVAL                300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION                0                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL              (uint16_t) MSEC_TO_UNITS(7.5, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL              (uint16_t) MSEC_TO_UNITS(8, UNIT_1_25_MS)          /**< Maximum acceptable connection interval (0.2 second). */
//#define MAX_CONN_INTERVAL             MSEC_TO_UNITS(8, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(1000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(50000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    6                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#define MAX_PACKET_SIZE                 200                                     //Maximum Packet size

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

//Defining Service UUIDS not already defined by nRF
#define BASE_UUID             {0xF0,0x00,0x00,0xF9,0x2E,0xEF,0x42,0x62,0x97,0xD1,0x74,0xAC,0x59,0x6E,0x44,0x92}  //Base UUID to serve as core of other UUIDS

#define SYSTEM_SERVICE_UUID            0x180F                                   //Battery Service defined by SIG
#define TUPPERWARE_SERVICE_UUID        0xAAAF                                   //UUID to be inserted into the 0x00 in the Base for 128 bit identifier

//Defining Chacteristic UUID
#define BATT_ST_CHAR_UUID             0xBA77                                    //UUID for Battery Chacteristic
#define SYS_ST_CHAR_UUID              0xBA78                                    //UUID for System State Chacteristic
#define TEMP_CHAR_UUID                0x7E39                                    //UUID for Temp Chacteristic
#define STEALTH_CHAR_UUID             0x7E41                                    //UUID for Stealth Chacteristic
#define IMAGE_CHAR_UUID               0x13A6                                    //UUID for Image Chacteristic
#define IMAGE_CHECKSUM_CHAR_UUID      0x13A7                                    //UUID for Image Checksum Chacteristic
#define LID_STATUS_CHAR_UUID          0x7E40                                    //UUID for Lid Image Characteristic

#define BLE_DEBUG_MODE 0


//All advertisement informatrion 
#define BEACON_INFO LENGTH
#define ADV_DATA_LENGTH
#define COMPANY_IDENTIFIER 0x0059 //USing Nordics for now


#include "platform.h"
#include "peer_manager.h"
#include "peer_manager_types.h"
#include "peer_manager_handler.h"

//This structure will hold all the data assigned to a characteristic
typedef struct
{
    ble_uuid_t                char_uuid;        //Characteristic UUID
    ble_gatts_char_md_t       char_md;          //Characteristic Metadata
    ble_gatts_attr_md_t       attr_md;          //Attribute Metadat
    ble_gatts_attr_md_t       cccd_md;          //Characteristic Metadata
    ble_gatts_attr_t          attr_char_value;  //Attribute characteric value metadata
    ble_gatts_char_handles_t  handles;          //Characteristic handles
    uint8_t CCCD[2];
}cust_characteristic_t;

//This structure will be used for holding all generalized service information
typedef struct
{
    ble_uuid_t                  service_uuid;                 /* Random Service UUID */
    uint16_t                    service_handle;               /*  provided by BLE Stack*/
    uint16_t                    conn_handle;                  /*  Identifies the device connected */
    cust_characteristic_t       temp_char;                    /*  Temperature characteristic's information */
    cust_characteristic_t       stealth_char;                 /*  Stealth characteristic's information */
    cust_characteristic_t       img_status_char;              /*  Image Status characteristic's information */
    cust_characteristic_t       image_char;                   /*  Image characteristic's information */
    cust_characteristic_t       image_chk_char;               /*  Image Chacksum characteristic's information */
    cust_characteristic_t       lid_char;                     /*  Image Chacksum characteristic's information */
 }Tupper_service_t;


//This structure will be used for holding all generalized service information
typedef struct
{
    ble_uuid_t                  service_uuid;             /* Random Service UUID */
    uint16_t                    service_handle;           /*provided by BLE Stack*/
    uint16_t                    conn_handle;              /* Identifies the device connected */
    cust_characteristic_t       battery_state;            /* Battery state characteristic's Information */
    cust_characteristic_t       device_state;            /* Battery state characteristic's Information */
}System_service_t;


void image_data_rec(ble_evt_t const * p_ble_evt);
static void reading_timer_event(void * p_context);
static void stealth_timer_event(void * p_context);
static void shutoff_timer_event(void * p_context);
void Configure_System_Service(void);
void Configure_Tupperware_Service(void);
void send_indication_battery(uint16_t handle, uint8_t * data);
void send_Indication_image_ChckSum(uint16_t handle, uint8_t * data);
void send_Indication_Lid(uint16_t handle, uint8_t * data);
void send_Indication_temp(uint16_t handle, uint8_t * data);
extern void update_stealth(uint8_t stealth_status);
void send_Indication_stealth(uint16_t handle, uint8_t * data);
void process_system_command(uint8_t data);

void send_adc1(uint16_t handle, uint8_t * data);
void send_adc2(uint16_t handle, uint8_t * data);

static void identities_set(pm_peer_id_list_skip_t skip);

extern void advertising_start(bool erase_bonds);
extern void log_init(void);
extern void timers_init(void);
extern void gap_params_init(void);
extern void gatt_init(void);
extern void services_init(void);
extern void conn_params_init(void);
extern void ble_stack_init(void);
extern void peer_manager_init(void);
extern void advertising_init(bool whitelist_enable);
extern void application_timers_start(void);
extern void power_management_init(void);
extern void idle_state_handle(void);
extern void buttons_leds_init(bool * p_erase_bonds);
extern void update_system_state(uint8_t * Status, uint8_t length);
extern void update_battery(bool charging, uint8_t charge);
extern void update_Img_Chk_Sum(uint8_t chk_sum);
extern void update_Lid(uint8_t Lid_St);
extern void update_battery_state(bool charging, uint8_t percent_charge);
extern void send_Indication_Battery(uint16_t handle, uint8_t *data);
#endif // BLUETOOTH_H__