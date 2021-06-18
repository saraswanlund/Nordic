#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "fds.h"
#include "peer_manager.h"
#include "peer_manager_types.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "ble_nus.h"
#include "app_util_platform.h"
#include "app_uart.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_delay.h"

#include "bluetooth.h"

#include "platform.h"
#include "app.h"
#include "SEGGER_RTT.h"

#include "main.h"
#include "system_commands.h"
#include "pl_i2c.h"
#include "eeprom-i2c.h"
#include "sensor_accel.h"
#include "sensor_als.h"
#include "battery_moniter.h"

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"
#include "nrf_power.h"

NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                             /**< Advertising module instance. */
APP_TIMER_DEF(reading_timer);

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
static uint8_t  temperature = 0;                                                // This is stores all temperature readings
static uint8_t  battery_status[2] = {100,1};                                    //Battery Status variable
static uint8_t  image_buffer[209];                                              //Image Buffer variable
static uint8_t  image_state[2];                                                 //Image State variable passed to the BLE stack 
static uint8_t  system_state[10];                                               //System State Variable
static uint8_t  system_state_buffer[10];                                         //Used to hold system state of internal device
static uint8_t  image_checksum;                                                 //Stores the Image checksum
static uint8_t  lid_status;                                                 //Stores the Image checksum

static uint8_t  adc1_buffer[2];
static uint8_t  adc2_buffer[2];

static uint16_t  buffer;
static bool connection_sett_negotation;
static bool whitelist_en;

static pm_peer_id_t      m_peer_id;

ble_gap_addr_t connected_Device;

uint8_t adv_data[] = {'S', 'W','K',0x00};

System_service_t   m_system_service;
Tupper_service_t m_tupp_service;

uint32_t temp_reading;                                                 //Stores the temp modules reading
uint8_t stealth_flagG = 0;
uint8_t stealth_timerG = 0;

struct pl_i2c host_i2c;
struct i2c_eeprom hw_eeprom = {
        &host_i2c, I2C_HWINFO_EEPROM_ADDR, EEPROM_AT214CM02,
};

struct i2c_als als_bt = {
    NULL, I2C_ALS_ADDR ,LTR_329ALS
};

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] =                                               /**< Universally unique service identifiers. */
{
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
    {SYSTEM_SERVICE_UUID, BLE_UUID_TYPE_BLE},
    {TUPPERWARE_SERVICE_UUID, BLE_UUID_TYPE_BLE}
};


void advertising_start(bool erase_bonds);
void delete_bonds(void);

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);


static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = buttonless_dfu_sdh_state_observer,
};

static void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout  = APP_ADV_DURATION;
}


static void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}



// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            advertising_config_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
            NRF_LOG_INFO("Connected to a previously bonded device.");
            break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
            NRF_LOG_INFO("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
            if(p_evt->params.conn_sec_succeeded.procedure == 1){    // if procedure is bonding, connect again to encyprtion procedure
                NRF_LOG_INFO("Connect Again for Encyprtion Procedure");
                pm_conn_secure(p_evt->conn_handle, false);           // only happens when Phone lost bonding info, and board needs to repair again
            }
            break;

        case PM_EVT_CONN_SEC_FAILED:
            NRF_LOG_INFO("Connection Security Failed");
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
            break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
            NRF_LOG_INFO("Allow Repairing Request from an Already Bonded Peer");
            // Allow pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};       
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
            break;

        case PM_EVT_STORAGE_FULL:
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_NO_SPACE_IN_QUEUES){
                // Retry.
            }
            else{
                APP_ERROR_CHECK(err_code);
            }
            break;
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            NRF_LOG_DEBUG("PM_EVT_PEERS_DELETE_SUCCEEDED");
            //advertising_start(false); // was advertising_start(false) before
            break;
        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            // Assert.
            //APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
            NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_FAILED");
            break;
        case PM_EVT_PEER_DELETE_FAILED:
            // Assert.
            //APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
            NRF_LOG_INFO("PM_EVT_PEER_DELETE_FAILED");
            break;
        case PM_EVT_PEERS_DELETE_FAILED:
            // Assert.
            //APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
            NRF_LOG_INFO("PM_EVT_PEERS_DELETE_FAILED");
            break;
        case PM_EVT_ERROR_UNEXPECTED:
            // Assert.
            //APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
            NRF_LOG_INFO("PM_EVT_ERROR_UNEXPECTED");
            break;
        case PM_EVT_CONN_SEC_START:
            NRF_LOG_INFO("Connecting Starts.");
            break;
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
          break;
        case PM_EVT_PEER_DELETE_SUCCEEDED:
          break;
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
          break;
        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
          // This can happen when the local DB has changed.
          break;
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
          break;  
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
          break;
        default:
          break;
    }
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one. */
       err_code = app_timer_create(&reading_timer, APP_TIMER_MODE_REPEATED, reading_timer_event);
       APP_ERROR_CHECK(err_code); 
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;  //empty structure containing connection parameters for the profile
    ble_gap_conn_sec_mode_t sec_mode; //empty structure containing the security modes. 


    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);  //set connection to an unprotected connection link.
                                                //The function sets the mode. There are 7 possible modes

    //Assign the device the name that was listed in the defintion above 
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(GAP_APPEARANCE);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params)); //reserve memory for connection parameters

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);   //Load connection parameters
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
   
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    ret_code_t         err_code;
    nrf_ble_qwr_init_t qwr_init = {0};
    ble_dfu_buttonless_init_t dfus_init = {0};      // DFU Service - 5.12.2020 SJE

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    //Initialize the system service
    ble_uuid128_t     base_uuid = BASE_UUID;
    m_system_service.service_uuid.uuid = SYSTEM_SERVICE_UUID;

    //Add the System service UUID to the table
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_system_service.service_uuid.type);
    APP_ERROR_CHECK(err_code);
  
    //Add the system service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &m_system_service.service_uuid, &m_system_service.service_handle);
    APP_ERROR_CHECK(err_code);

    //Set up the characteristics bound to the system service
    Configure_System_Service();

    //Add Tupperware Service
    m_tupp_service.service_uuid.uuid = TUPPERWARE_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_tupp_service.service_uuid.type);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &m_tupp_service.service_uuid,
                                        &m_tupp_service.service_handle);
    //Configure the Tupperware service
    Configure_Tupperware_Service();

    
    // DFU Service - 5.12.2020 SJE
    dfus_init.evt_handler = ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
//        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
//        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
void application_timers_start(void)
{
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer. */
       ret_code_t err_code;
       err_code = app_timer_start(reading_timer, APP_TIMER_TICKS(3000), NULL); //Call the timer interrupt every 3s
       APP_ERROR_CHECK(err_code); 

//       err_code = app_timer_start(stealth_timer, APP_TIMER_TICKS(1000), NULL); //Call the timer interrupt every 1s
//       APP_ERROR_CHECK(err_code); 
//
//       err_code = app_timer_start(shutoff_timer, APP_TIMER_TICKS(60000), NULL); //Call the timer interrupt every 60s
//       APP_ERROR_CHECK(err_code); 
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
//    err_code = bsp_btn_ble_sleep_mode_prepare();
//    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        case BLE_ADV_EVT_WHITELIST_REQUEST:
            {
            // When the Advertising module is about to advertise, an event
            // will be received by the application. In this event, the application
            // retrieves a whitelist from the Peer Manager, based on the peers
            // previously whitelisted using pm_whitelist_set().
            ret_code_t err_code;
            // Storage for the whitelist.
            printf("Whitelist Request\n");
            ble_gap_irk_t  irks[8]  = {0};
            ble_gap_addr_t addrs[8] = {0};
            uint32_t irk_cnt  = 8;
            uint32_t addr_cnt = 8;
            err_code = pm_whitelist_get(addrs, &addr_cnt, irks, &irk_cnt);
            APP_ERROR_CHECK(err_code);
            // Apply the whitelist.
            err_code = ble_advertising_whitelist_reply(&m_advertising,addrs, addr_cnt, irks, irk_cnt);
            identities_set(PM_PEER_ID_LIST_SKIP_NO_IRK);
            APP_ERROR_CHECK(err_code);
            break;
          }
         case BLE_ADV_EVT_FAST_WHITELIST:
             printf("Fast advertising with whitelisting started\n");
             ble_gap_addr_t  p_addrs[2];
             uint32_t addr_count = 2;
             ble_gap_irk_t  p_irks[2];
             uint32_t irk_count = 2;

             err_code = pm_whitelist_get(p_addrs,&addr_count,p_irks,&irk_count);
             printf("Whitelist Addrs: %d, IRKS: %d\n",addr_count,irk_count);
             break;
        case BLE_ADV_EVT_PEER_ADDR_REQUEST:
        {
            pm_peer_data_bonding_t peer_bonding_data;

            // Only Give peer address if we have a handle to the bonded peer.
            if (m_peer_id != PM_PEER_ID_INVALID)
            {
                err_code = pm_peer_data_bonding_load(m_peer_id, &peer_bonding_data);
                if (err_code != NRF_ERROR_NOT_FOUND)
                {
                    APP_ERROR_CHECK(err_code);

                    // Manipulate identities to exclude peers with no Central Address Resolution.
                    identities_set(PM_PEER_ID_LIST_SKIP_ALL);

                    ble_gap_addr_t * p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info);
                    err_code = ble_advertising_peer_addr_reply(&m_advertising, p_peer_addr);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break;
        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("BLE GAP EVT Disconnected.");
            // LED indication will be changed when advertising starts.
            #if BLE_DEBUG_MODE == 1
              //Print who disconnected and the reason
              SEGGER_RTT_printf(0,"BLE GAP Disconnect, Handle: %x\n Reason: %x\n", p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gap_evt.params.disconnected.reason);
            #endif
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            BT_connectionG = 0;
            advertising_start(false);
            //err_code = app_timer_start(shutoff_timer, APP_TIMER_TICKS(60000), NULL);
            break;
        case BLE_GAP_EVT_CONNECTED:
            #if BLE_DEBUG_MODE == 1
              SEGGER_RTT_printf(0,"BLE GAP Connected, Handle: %x\n", p_ble_evt->evt.gap_evt.conn_handle);
            #endif
            NRF_LOG_INFO("BLE GAP EVT Connected.");
            connection_sett_negotation = true;
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            connected_Device = p_ble_evt->evt.gap_evt.params.connected.peer_addr;
            //Ask and select for the fastest version of BLE possible 
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_1MBPS | BLE_GAP_PHY_2MBPS,
                .tx_phys = BLE_GAP_PHY_1MBPS | BLE_GAP_PHY_2MBPS,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
            //err_code = sd_ble_gattc_exchange_mtu_request(p_ble_evt->evt.gap_evt.conn_handle, MAX_PACKET_SIZE);
            nrf_pwr_mgmt_feed();                         //Prevent power management from disconnecting anything
            BT_connectionG = 1;
            //app_timer_stop(shutoff_timer);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
            connection_sett_negotation = true;
        } break;

        case BLE_GAP_EVT_PHY_UPDATE:
        {
           printf("PHY Update: Connection set \n");
           connection_sett_negotation = false;
           #if BLE_DEBUG_MODE == 1
              SEGGER_RTT_printf(0,"PHY Update: Connection set \n");
           #endif
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.

            #if BLE_DEBUG_MODE == 1
              SEGGER_RTT_printf(0,"BLE GATT EVENT Client Timeout\n");
            #endif

            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.

            #if BLE_DEBUG_MODE == 1
              //printf("BLE GATT Server Timeout\n");
              SEGGER_RTT_printf(0,"BLE GATT Server Timeout\n");
            #endif
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

         case BLE_GATTS_EVT_WRITE:

            if(p_ble_evt->evt.gatts_evt.params.write.uuid.uuid == m_tupp_service.image_char.char_uuid.uuid)
            {     
                  
            }else if(p_ble_evt->evt.gatts_evt.params.write.uuid.uuid == m_system_service.device_state.char_uuid.uuid)
            {
                
            }
            break;

         case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
             connection_sett_negotation = true;
             err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle, MAX_PACKET_SIZE);
             APP_ERROR_CHECK(err_code);
            break;

         case BLE_GATTS_EVT_HVN_TX_COMPLETE:
              printf("Notification Sent\n");
            break;

         case BLE_GATTS_EVT_HVC:
            if(p_ble_evt->evt.gatts_evt.params.hvc.handle == m_system_service.battery_state.handles.value_handle){
              printf("ADC2 indication acknowledged\n");
            } else if(p_ble_evt->evt.gatts_evt.params.hvc.handle == m_tupp_service.image_chk_char.handles.value_handle){
              printf("Image Checksum indication acknowledged\n");
            } else if(p_ble_evt->evt.gatts_evt.params.hvc.handle == m_tupp_service.lid_char.handles.value_handle){
              printf("ADC1 indication acknowledged\n");
            } else if(p_ble_evt->evt.gatts_evt.params.hvc.handle == m_tupp_service.stealth_char.handles.value_handle){
              printf("Stealth indication acknowledged\n");
            } else if(p_ble_evt->evt.gatts_evt.params.hvc.handle == m_tupp_service.temp_char.handles.value_handle){
              printf("Temp indication acknowledged\n");
            }
            break;
          case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
            printf("MTU updated\n"); 
            connection_sett_negotation = false;
            break;
    
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init()
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();  //Enables the Softdevice for BLE
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    //APP_BLE_CONN_CFG_TAG is a huge Union of all possible setting structures
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start); //Load settings in BLE stack
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

}


/**@brief Function for the Peer Manager initialization.
 */
void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;  //Enable Bonding
    sec_param.mitm           = SEC_PARAM_MITM;  //No Man in the middle Protection
    sec_param.lesc           = SEC_PARAM_LESC;  //No LE Secure Pairing
    sec_param.keypress       = SEC_PARAM_KEYPRESS; //No keypress notifications
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES; //No IO Capabilities
    sec_param.oob            = SEC_PARAM_OOB;   //No out of band authentication abilities
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE; //Seven octects for encryption key
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE; //16 octects for maximum encryption key
    sec_param.kdist_own.enc  = 1;   //Will provide long term encryption key along with master identification
    sec_param.kdist_own.id   = 1;   // Will provice IRK
    sec_param.kdist_peer.enc = 1;   //Willing to store long term encryption
    sec_param.kdist_peer.id  = 1;   //Willing to sstore IRK

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Clear bond information from persistent storage.
 */
void delete_bonds(void)
{
    ret_code_t err_code;
    if(pm_peer_count() !=0){
    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
    printf("Erase Bonds Completed\n");
    }
}


/**@brief Function for initializing the Advertising functionality. 
  * Using basic securityless connections. Later whitelisting will be used to filter connections
 */
void advertising_init(bool whitelist_enable)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;

    ble_advdata_manuf_data_t manuf_data;
    manuf_data.company_identifier = COMPANY_IDENTIFIER;
    
    manuf_data.data.size = sizeof(adv_data);
    manuf_data.data.p_data = adv_data;
  
    init.advdata.p_manuf_specific_data = &manuf_data;
    init.evt_handler = on_adv_evt;

    if(whitelist_enable)
    {
    init.config.ble_adv_whitelist_enabled = 1;
    adv_data[3] = 0x01;
    }else
    {
      init.config.ble_adv_whitelist_enabled = 0;
      adv_data[3] = 0x00;
    }

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing the nrf log module.
 */
void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Function for starting advertising.
 */
void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETED_SUCEEDED event
    }
    else
    {
        ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
    }

}




void update_system_state(uint8_t * Status, uint8_t length)
{
  for(uint8_t i =0; i< length; i++)
  {
      system_state[i] = Status[i];
      system_state_buffer[i] = Status[i]; //Hold state that can be used to restore the system's state after a command is written to the characteristic.
  }
  
}

void Configure_System_Service(void)
{
    /*       Battery State Chacteristic
    *  UUID: 9244BA77-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Read, Indication
    *  Size: 2 Bytes
    *  Read Description: Byte 1 Battery Charge
    *	                 Byte 2 Charging/Discharging
    *  Indication: Change in Byte 2 or Critical Battery 
    */

     ret_code_t         err_code;
     ble_uuid128_t     base_uuid = BASE_UUID;
    //Add the battery state characteristic UUID to the table
    m_system_service.battery_state.char_uuid.uuid = BATT_ST_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_system_service.battery_state.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the battery state chacteristic attribute metadata
    memset(&m_system_service.battery_state.attr_md, 0, sizeof(m_system_service.battery_state.attr_md)); // Clear data in structure
    m_system_service.battery_state.attr_md.read_perm.lv = 1;
    m_system_service.battery_state.attr_md.read_perm.sm = 1;            //Setting the Security mode to Mode 1, Level 1 -> No security
    m_system_service.battery_state.attr_md.write_perm.sm = 1;           //Making sure that this cannot be written to with Mode 0
    m_system_service.battery_state.attr_md.write_perm.lv = 1;
    m_system_service.battery_state.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the characteristic attribute value 
    memset(&m_system_service.battery_state.attr_char_value, 0, sizeof(m_system_service.battery_state.attr_char_value));
    m_system_service.battery_state.attr_char_value.max_len = 2;         //Maximum length of attribure is 2 Byte
    m_system_service.battery_state.attr_char_value.p_uuid = &m_system_service.battery_state.char_uuid;  //Points to the UUID;
    m_system_service.battery_state.attr_char_value.p_attr_md = &m_system_service.battery_state.attr_md; //Points to the attribute metadata
    m_system_service.battery_state.attr_char_value.p_value = adc2_buffer; //attach pointer to our battery status variable
    
    //Setting up the CCCD metadata
    memset(&m_system_service.battery_state.cccd_md, 0, sizeof(m_system_service.battery_state.cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_system_service.battery_state.cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_system_service.battery_state.cccd_md.write_perm);
    m_system_service.battery_state.cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    //Set up characteristic W/R permissions
    memset(&m_system_service.battery_state.char_md, 0, sizeof(m_system_service.battery_state.char_md));
    m_system_service.battery_state.char_md.char_props.read = 1;  //Allow Reading of attribute
    m_system_service.battery_state.char_md.char_props.indicate = 1;  //Flags indication

    m_system_service.battery_state.char_md.p_cccd_md= &m_system_service.battery_state.cccd_md; //Points to the CCCD meta Data



    err_code = sd_ble_gatts_characteristic_add(m_system_service.service_handle, 
                                            &m_system_service.battery_state.char_md, 
                                            &m_system_service.battery_state.attr_char_value, 
                                            &m_system_service.battery_state.handles);
    APP_ERROR_CHECK(err_code);
  




    /*       System State Chacteristic
    *  UUID: 9244BA78-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Read, Write
    *  Size: 2 Bytes
    *  Read Description: Byte 1 Device Error Code
    *	                 Byte 2 Device Power State
    *  Write: System Command 
    */

    //Add the System state characteristic UUID to the table
    m_system_service.device_state.char_uuid.uuid = SYS_ST_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_system_service.device_state.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the system state chacteristic attribute metadata
    memset(&m_system_service.device_state.attr_md, 0, sizeof(m_system_service.device_state.attr_md)); // Clear data in structure
    m_system_service.device_state.attr_md.read_perm.lv = 1;
    m_system_service.device_state.attr_md.read_perm.sm = 1;            //Setting the Security mode to Mode 1, Level 1 -> No security
     m_system_service.device_state.attr_md.write_perm.lv = 1;
    m_system_service.device_state.attr_md.write_perm.sm = 1;           //Making sure that this cannot be written to with Mode 0
    m_system_service.device_state.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the characteristic attribute value 
    memset(&m_system_service.device_state.attr_char_value, 0, sizeof(m_system_service.device_state.attr_char_value));
    m_system_service.device_state.attr_char_value.max_len = sizeof(system_state);         //Maximum length of attribure is 1 Byte
    m_system_service.device_state.attr_char_value.p_uuid = &m_system_service.device_state.char_uuid;  //Points to the UUID;
    m_system_service.device_state.attr_char_value.p_attr_md = &m_system_service.device_state.attr_md; //Points to the attribure metadata
   
    m_system_service.device_state.attr_char_value.p_value = system_state; //attach pointer to our temperature variable

    //Set up characteristic W/R permissions
    memset(&m_system_service.device_state.char_md, 0, sizeof(m_system_service.device_state.char_md));
    m_system_service.device_state.char_md.char_props.read = 1;  //Allow Reading of attribute
    m_system_service.device_state.char_md.char_props.write = 1;  //Allow Reading of attribute

    err_code = sd_ble_gatts_characteristic_add(m_system_service.service_handle, 
                                            &m_system_service.device_state.char_md, 
                                            &m_system_service.device_state.attr_char_value, 
                                            &m_system_service.device_state.handles);
    APP_ERROR_CHECK(err_code);

}

void Configure_Tupperware_Service(void)
{
    ret_code_t         err_code;
    ble_uuid128_t     base_uuid = BASE_UUID;

     /*       Stealth Characteristic
    *  UUID: 92447E41-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Read
    *  Size: 1 Byte
    *  Read Description: Stealth status
    */

    //Add the Temperature characteristic UUID to the table
    m_tupp_service.stealth_char.char_uuid.uuid = STEALTH_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_tupp_service.stealth_char.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the Temperature chacteristic attribute metadata
    memset(&m_tupp_service.stealth_char.attr_md, 0, sizeof(m_tupp_service.stealth_char.attr_md)); // Clear data in structure
    m_tupp_service.stealth_char.attr_md.read_perm.lv = 1;
    m_tupp_service.stealth_char.attr_md.read_perm.sm = 1;            //Setting the Security mode to Mode 1, Level 1 -> No security
    m_tupp_service.stealth_char.attr_md.write_perm.sm = 0;           //Making sure that this cannot be written to with Mode 0
    m_tupp_service.stealth_char.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the Temperature characteristic attribute value 
    memset(&m_tupp_service.stealth_char.attr_char_value, 0, sizeof(m_tupp_service.stealth_char.attr_char_value));
    m_tupp_service.stealth_char.attr_char_value.max_len = sizeof(temperature);         //Maximum length of attribure is 1 Byte
    m_tupp_service.stealth_char.attr_char_value.p_uuid = &m_tupp_service.stealth_char.char_uuid;  //Points to the UUID;
    m_tupp_service.stealth_char.attr_char_value.p_attr_md = &m_tupp_service.stealth_char.attr_md; //Points to the attribure metadata
    m_tupp_service.stealth_char.attr_char_value.p_value = &temperature; //attach pointer to our temperature variable

    //Setting up the CCCD metadata
    memset(&m_tupp_service.stealth_char.cccd_md, 0, sizeof(m_tupp_service.stealth_char.cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.stealth_char.cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.stealth_char.cccd_md.write_perm);
    m_tupp_service.stealth_char.cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    m_tupp_service.stealth_char.char_md.p_cccd_md= &m_tupp_service.stealth_char.cccd_md; //Points to the CCCD meta Data

    //Set up the temperature characteristic W/R permissions
    memset(&m_tupp_service.stealth_char.char_md, 0, sizeof(m_tupp_service.stealth_char.char_md));
    m_tupp_service.stealth_char.char_md.char_props.read = 1;  //Allow Reading of attribute
    m_tupp_service.stealth_char.char_md.char_props.indicate = 1;  

    //Add the characteristic 
    err_code = sd_ble_gatts_characteristic_add(m_tupp_service.service_handle, 
                                            &m_tupp_service.stealth_char.char_md, 
                                            &m_tupp_service.stealth_char.attr_char_value, 
                                            &m_tupp_service.stealth_char.handles);
    APP_ERROR_CHECK(err_code);
    

     /*       Temperature Characteristic
    *  UUID: 92447E39-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Read
    *  Size: 1 Byte
    *  Read Description: Temperature in Fahrenheit 
    */

    //Add the Temperature characteristic UUID to the table
    m_tupp_service.temp_char.char_uuid.uuid = TEMP_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_tupp_service.temp_char.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the Temperature chacteristic attribute metadata
    memset(&m_tupp_service.temp_char.attr_md, 0, sizeof(m_tupp_service.temp_char.attr_md)); // Clear data in structure
    m_tupp_service.temp_char.attr_md.read_perm.lv = 1;
    m_tupp_service.temp_char.attr_md.read_perm.sm = 1;            //Setting the Security mode to Mode 1, Level 1 -> No security
    m_tupp_service.temp_char.attr_md.write_perm.sm = 0;           //Making sure that this cannot be written to with Mode 0
    m_tupp_service.temp_char.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the Temperature characteristic attribute value 
    memset(&m_tupp_service.temp_char.attr_char_value, 0, sizeof(m_tupp_service.temp_char.attr_char_value));
    m_tupp_service.temp_char.attr_char_value.max_len = sizeof(temperature);         //Maximum length of attribure is 1 Byte
    m_tupp_service.temp_char.attr_char_value.p_uuid = &m_tupp_service.temp_char.char_uuid;  //Points to the UUID;
    m_tupp_service.temp_char.attr_char_value.p_attr_md = &m_tupp_service.temp_char.attr_md; //Points to the attribure metadata
    m_tupp_service.temp_char.attr_char_value.p_value = &temperature; //attach pointer to our temperature variable

    //Setting up the CCCD metadata
    memset(&m_tupp_service.temp_char.cccd_md, 0, sizeof(m_tupp_service.temp_char.cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.temp_char.cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.temp_char.cccd_md.write_perm);
    m_tupp_service.temp_char.cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    m_tupp_service.temp_char.char_md.p_cccd_md= &m_tupp_service.temp_char.cccd_md; //Points to the CCCD meta Data

    //Set up the temperature characteristic W/R permissions
    memset(&m_tupp_service.temp_char.char_md, 0, sizeof(m_tupp_service.temp_char.char_md));
    m_tupp_service.temp_char.char_md.char_props.read = 1;  //Allow Reading of attribute
    m_tupp_service.temp_char.char_md.char_props.indicate = 1;  

    //Add the characteristic 
    err_code = sd_ble_gatts_characteristic_add(m_tupp_service.service_handle, 
                                            &m_tupp_service.temp_char.char_md, 
                                            &m_tupp_service.temp_char.attr_char_value, 
                                            &m_tupp_service.temp_char.handles);
    APP_ERROR_CHECK(err_code);

    /*       Image Receive Characteristic
    *  UUID: 924413A6-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Write w/o response
    *  Size: 200 Bytes
    *  Write: Write image data to send to display 
    */
    
     //Add the image receiving chacteristic UUID to the table
    m_tupp_service.image_char.char_uuid.uuid = IMAGE_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_tupp_service.image_char.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the image receiving chacteristic attribute metadata
    memset(&m_tupp_service.image_char.attr_md, 0, sizeof(m_tupp_service.image_char.attr_md)); // Clear data in structure
    m_tupp_service.image_char.attr_md.read_perm.lv = 0;
    m_tupp_service.image_char.attr_md.read_perm.sm = 0;            //Setting the Security mode to Mode 1, Level 1 -> No security
    m_tupp_service.image_char.attr_md.write_perm.sm = 1;           //Making sure that this can we written to with no security 
    m_tupp_service.image_char.attr_md.write_perm.lv = 1;            
    m_tupp_service.image_char.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the image receiving chacteristic attribute value 
    memset(&m_tupp_service.image_char.attr_char_value, 0, sizeof(m_tupp_service.image_char.attr_char_value));
    m_tupp_service.image_char.attr_char_value.max_len = sizeof(image_buffer);         //Maximum length of attribute to buffer size
    m_tupp_service.image_char.attr_char_value.p_uuid = &m_tupp_service.image_char.char_uuid;  //Points to the UUID;
    m_tupp_service.image_char.attr_char_value.p_attr_md = &m_tupp_service.image_char.attr_md; //Points to the attribure metadata
    m_tupp_service.image_char.attr_char_value.p_value = image_buffer; //attach pointer to our image data buffer variable

    //Set up the Image Receiver characteristic W/R permissions
    memset(&m_tupp_service.image_char.char_md, 0, sizeof(m_tupp_service.image_char.char_md));
    m_tupp_service.image_char.char_md.char_props.read = 0;  //Disallow Reading of attribute
    m_tupp_service.image_char.char_md.char_props.write = 0;
    m_tupp_service.image_char.char_md.char_props.write_wo_resp = 1;

    //Add the characteristic 
    err_code = sd_ble_gatts_characteristic_add(m_tupp_service.service_handle, 
                                            &m_tupp_service.image_char.char_md, 
                                            &m_tupp_service.image_char.attr_char_value, 
                                            &m_tupp_service.image_char.handles);
    APP_ERROR_CHECK(err_code);


     /*       Image Checksum Characteristic
    *  UUID: 92447A7-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Read, Indication
    *  Size: 1 Byte
    *  Read Description: Image Checksum  
    *  Indication: Sends new running checksum during transfer 
    */
    
     //Add the image checksum chacteristic UUID to the table
    m_tupp_service.image_chk_char.char_uuid.uuid = IMAGE_CHECKSUM_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_tupp_service.image_chk_char.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the image checksum chacteristic attribute metadata
    memset(&m_tupp_service.image_chk_char.attr_md, 0, sizeof(m_tupp_service.image_chk_char.attr_md)); // Clear data in structure
    m_tupp_service.image_chk_char.attr_md.read_perm.lv = 1;
    m_tupp_service.image_chk_char.attr_md.read_perm.sm = 1;            //Setting the Security mode to Mode 1, Level 1 -> No security
    m_tupp_service.image_chk_char.attr_md.write_perm.sm = 0;           //Making sure that this can we written to with no security 
    m_tupp_service.image_chk_char.attr_md.write_perm.lv = 0;            
    m_tupp_service.image_chk_char.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the image receiving chacteristic attribute value 
    memset(&m_tupp_service.image_chk_char.attr_char_value, 0, sizeof(m_tupp_service.image_chk_char.attr_char_value));
    m_tupp_service.image_chk_char.attr_char_value.max_len = sizeof(image_checksum);         //Maximum length of attribute to buffer size
    m_tupp_service.image_chk_char.attr_char_value.p_uuid = &m_tupp_service.image_chk_char.char_uuid;  //Points to the UUID;
    m_tupp_service.image_chk_char.attr_char_value.p_attr_md = &m_tupp_service.image_chk_char.attr_md; //Points to the attribure metadata
    m_tupp_service.image_chk_char.attr_char_value.p_value = &image_checksum; //attach pointer to our image data buffer variable

        //Setting up the CCCD metadata
    memset(&m_tupp_service.image_chk_char.cccd_md, 0, sizeof(m_tupp_service.image_chk_char.cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.image_chk_char.cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.image_chk_char.cccd_md.write_perm);
    m_tupp_service.image_chk_char.cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    m_tupp_service.image_chk_char.char_md.p_cccd_md= &m_tupp_service.image_chk_char.cccd_md; //Points to the CCCD meta Data

    //Set up the Image Receiver characteristic W/R permissions
    memset(&m_tupp_service.image_chk_char.char_md, 0, sizeof(m_tupp_service.image_chk_char.char_md));
    m_tupp_service.image_chk_char.char_md.char_props.read = 1;  //allow Reading of attribute
    m_tupp_service.image_chk_char.char_md.char_props.indicate = 0;
    m_tupp_service.image_chk_char.char_md.char_props.notify = 1;
    


    //Add the characteristic 
    err_code = sd_ble_gatts_characteristic_add(m_tupp_service.service_handle, 
                                            &m_tupp_service.image_chk_char.char_md, 
                                            &m_tupp_service.image_chk_char.attr_char_value, 
                                            &m_tupp_service.image_chk_char.handles);
    APP_ERROR_CHECK(err_code);

    /*       Lid Status Characteristic
    *  UUID: 92447E40-AC74-D197-6242-EF2EF90000F0
    *  Attributes: Read, Indication
    *  Size: 2 Byte
    *  Read Description: Suspected Lid Status  
    *  Indicate: Sends change upon change
    */
    
     //Add the lid status chacteristic UUID to the table
    m_tupp_service.lid_char.char_uuid.uuid = LID_STATUS_CHAR_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_tupp_service.lid_char.char_uuid.type);
    APP_ERROR_CHECK(err_code);

    //Configure the lid status chacteristic attribute metadata
    memset(&m_tupp_service.lid_char.attr_md, 0, sizeof(m_tupp_service.lid_char.attr_md)); // Clear data in structure
    m_tupp_service.lid_char.attr_md.read_perm.lv = 1;
    m_tupp_service.lid_char.attr_md.read_perm.sm = 1;            //Setting the Security mode to Mode 1, Level 1 -> No security
    m_tupp_service.lid_char.attr_md.write_perm.sm = 0;           //Making sure that this can we written to with no security 
    m_tupp_service.lid_char.attr_md.write_perm.lv = 0;            
    m_tupp_service.lid_char.attr_md.vloc = BLE_GATTS_VLOC_USER;  //Telling system that the program will buffer the memory not the system

    //Setting up the lid status chacteristic attribute value 
    memset(&m_tupp_service.lid_char.attr_char_value, 0, sizeof(m_tupp_service.lid_char.attr_char_value));
    m_tupp_service.lid_char.attr_char_value.max_len = sizeof(2);         //Maximum length of attribute to buffer size
    m_tupp_service.lid_char.attr_char_value.p_uuid = &m_tupp_service.lid_char.char_uuid;  //Points to the UUID;
    m_tupp_service.lid_char.attr_char_value.p_attr_md = &m_tupp_service.lid_char.attr_md; //Points to the attribure metadata
    m_tupp_service.lid_char.attr_char_value.p_value = adc1_buffer; //attach pointer to our image data buffer variable

    memset(&m_tupp_service.lid_char.cccd_md, 0, sizeof(m_tupp_service.lid_char.cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.lid_char.cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&m_tupp_service.lid_char.cccd_md.write_perm);
    m_tupp_service.lid_char.cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    m_tupp_service.lid_char.char_md.p_cccd_md= &m_tupp_service.lid_char.cccd_md; //Points to the CCCD meta Data

    //Set up the lid status characteristic W/R permissions
    memset(&m_tupp_service.lid_char.char_md, 0, sizeof(m_tupp_service.lid_char.char_md));
    m_tupp_service.lid_char.char_md.char_props.read = 1;  //allow Reading of attribute
    m_tupp_service.lid_char.char_md.char_props.indicate = 1;

    //Add the characteristic 
    err_code = sd_ble_gatts_characteristic_add(m_tupp_service.service_handle, 
                                            &m_tupp_service.lid_char.char_md, 
                                            &m_tupp_service.lid_char.attr_char_value, 
                                            &m_tupp_service.lid_char.handles);
    APP_ERROR_CHECK(err_code);


}

static void reading_timer_event(void * p_context)
{
  ret_code_t err_code;
  uint8_t temp_byte;
  uint32_t voltage;
  uint8_t bat_percent;

  sd_temp_get(&temp_reading);
  APP_ERROR_CHECK(err_code);

  temperature = (((temp_reading/4) * 9)/5) +32; //Convert the temperature reading into Farenheight

  temp_byte = (uint8_t) nrf_gpio_pin_read(BAT_CHARGE_ST);
  //voltage = batt_mon_read_voltage();
  //bat_percent = (100 * voltage / 41000);
  printf("temp: %d \t voltage: %d \t percent: %d\n", temperature, voltage, bat_percent);

  if(BT_connectionG == 1){
      send_Indication_temp(m_conn_handle, &temperature);
      /*nrf_delay_ms(200);
      send_adc1(m_conn_handle, adc1);
      nrf_delay_ms(200);
      send_adc2(m_conn_handle, adc2);*/
      //update_Lid(ALS_get_data(&als_bt));

//      if(bat_percent > 1){
//          update_battery_state(temp_byte, voltage);
//      }
  }
}

void send_Indication_temp(uint16_t handle, uint8_t * data)
{
  if(m_conn_handle != BLE_CONN_HANDLE_INVALID)
  { 
    ble_gatts_value_t value;
    uint8_t CCCD[2] = {0,0};
    value.p_value = CCCD;
    value.len = 2;

       if(!connection_sett_negotation)
       {
          
         ret_code_t         err_code;
         buffer = 1;
         ble_gatts_hvx_params_t hvx;
         hvx.handle = m_tupp_service.temp_char.handles.value_handle;
         hvx.type = BLE_GATT_HVX_INDICATION;
         hvx.p_len = &buffer;
         hvx.offset = 0;
         hvx.p_data = data;
         err_code = sd_ble_gatts_hvx(handle,&hvx);
         if(err_code == NRF_ERROR_BUSY)
         {
           printf("Temp IND: Indication is busy \n");
         }
         //APP_ERROR_CHECK(err_code);
  
       }else
       {
         printf("Temp IND: MTU Update blocking indication\n");
       }
   }else
   {
    printf("Temp IND: Connection is invalid\n");

   }
}


void update_battery_state(bool charging, uint8_t charge)
{
    battery_status[0] = charging;
    battery_status[1] = charge;
//    send_indication_battery(m_conn_handle, battery_status);
}

void process_system_command(uint8_t  data)
{
    ret_code_t err_code;
    switch(system_state[0]){

    case PRIVATE_MODE:
    {
        pm_peer_id_t peer_ids[8] = {PM_PEER_ID_INVALID};
        uint32_t     n_peer_ids  = 0;
        pm_peer_id_t peer_id     = pm_next_peer_id_get(PM_PEER_ID_INVALID);
        while((peer_id != PM_PEER_ID_INVALID) && (n_peer_ids < 8))
        {
            peer_ids[n_peer_ids++] = peer_id;
            peer_id = pm_next_peer_id_get(peer_id);
        }
        // Whitelist peers.
        printf("Whitelisting: %d peers\n",n_peer_ids);
        err_code = pm_whitelist_set(peer_ids, n_peer_ids);
        APP_ERROR_CHECK(err_code);
        printf("Whitelisting Enabled\n");
        advertising_init(true);
    }
      break;
    case PUBLIC_MODE:
      if(pm_peer_count() !=0){
        delete_bonds();
        printf("Whitelisting Disabled\n");
      }
      advertising_init(false);
      break;
    default:
      printf("System command not recognized\n");
      break;
             
  }
  memcpy(system_state_buffer, system_state, sizeof(system_state)); //copy overwritten information back
}


static void identities_set(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT];
    uint32_t     peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT;

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
    APP_ERROR_CHECK(err_code);

    err_code = pm_device_identities_list_set(peer_ids, peer_id_count);
    APP_ERROR_CHECK(err_code);
}

static void shutoff_timer_event(void * p_context)
{
//    Status_LED(0); 
    printf("Shutoff Timer event\n");
//    sd_power_system_off();
}

void send_adc1(uint16_t handle, uint8_t * data)
{
  if(m_conn_handle != BLE_CONN_HANDLE_INVALID)
  { 
    adc1_buffer[0] = data[0];
    adc1_buffer[1] = data[1];
    printf("BT Channel 1 = %d \n ", adc1_buffer[0] + (adc1_buffer[1] << 8));

    ble_gatts_value_t value;
    uint8_t CCCD[3] = {0,0,0};
    value.p_value = CCCD;
    value.len = 3;
 
       if(!connection_sett_negotation)
       {
         ret_code_t         err_code;
         buffer = 2;
         ble_gatts_hvx_params_t hvx;
         hvx.handle = m_tupp_service.lid_char.handles.value_handle;
         hvx.type = BLE_GATT_HVX_INDICATION;
         hvx.p_len = &buffer;
         hvx.offset = 0;
         hvx.p_data = data;
         err_code = sd_ble_gatts_hvx(handle,&hvx);
         if(err_code == NRF_ERROR_BUSY)
         {
           printf("ADC1 IND using lid_char: Indication is busy \n");
         }
         //APP_ERROR_CHECK(err_code);
       }else
       {
         printf("ADC1 IND using lid_char: MTU Update blocking indication\n");
       }
   }else
   {
    printf("ADC1 IND using lid_char: Connection is invalid\n");
   }
}

void send_adc2(uint16_t handle, uint8_t * data)
{
  if(m_conn_handle != BLE_CONN_HANDLE_INVALID)
  { 
    adc2_buffer[0] = data[0];
    adc2_buffer[1] = data[1];
    printf("BT Channel 2 = %d \n ", adc2_buffer[0] + (adc2_buffer[1] << 8));

    ble_gatts_value_t value;
    uint8_t CCCD[3] = {0,0,0};
    value.p_value = CCCD;
    value.len = 3;
   // sd_ble_gatts_value_get(handle,m_tupp_service.image_chk_char.handles.cccd_handle, &value);
   
       if(!connection_sett_negotation)
       {
  
         ret_code_t         err_code;
         buffer = 2;
         ble_gatts_hvx_params_t hvx;
         hvx.handle = m_system_service.battery_state.handles.value_handle;
         hvx.type = BLE_GATT_HVX_INDICATION;
         hvx.p_len = &buffer;
         hvx.offset = 0;
         hvx.p_data = data;
         err_code = sd_ble_gatts_hvx(handle,&hvx);
         if(err_code == NRF_ERROR_BUSY)
         {
           printf("ADC2 IND using battery_state: Indication is busy \n");
         }
         //APP_ERROR_CHECK(err_code);
       }else
       {
         printf("ADC2 IND using battery_state: MTU Update blocking indication\n");
       }
   }else
   {
    printf("ADC2 IND using battery_state: Connection is invalid\n");

   }
}