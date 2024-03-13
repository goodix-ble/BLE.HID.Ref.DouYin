/**
 *****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief User function Implementation.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "user_app.h"
#include "user_periph_setup.h"
#include "user_mouse.h"
#include "hids.h"
#include "bas.h"
#include "dis.h"
#include "grx_sys.h"
#include "sensorsim.h"
#include "app_timer.h"
#include "app_log.h"
#include "utility.h"
#include "app_error.h"
#include "ble_advertising.h"
#include "ble_connect.h"
#include "ble_error.h"
#include "app_key.h"

#include "board_SK.h"


/*
 * DEFINES
 *****************************************************************************************
 */

/**@brief Gapm config data. */
#define DEVICE_NAME                         "Goodix_DY"         /**< Device Name which will be set in GAP. */
#define ADV_FAST_MIN_INTERVAL               40                  /**< The fast advertising min interval (in units of 0.625 ms). */
#define ADV_FAST_MAX_INTERVAL               40                  /**< The fast advertising max interval (in units of 0.625 ms). */
#define ADV_FAST_DURATION                   0               /**< The advertising timeout in units of 10ms. */





/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

static const uint8_t s_adv_data_set[] =                 /**< Advertising data. */
{
    0x0A,   // Length of this data
    BLE_GAP_AD_TYPE_COMPLETE_NAME,
    'G', 'o', 'o', 'd', 'i', 'x', '_', 'D', 'Y',

    0x03,
    BLE_GAP_AD_TYPE_APPEARANCE,
    LO_U16(BLE_APPEARANCE_HID_KEYBOARD),
    HI_U16(BLE_APPEARANCE_HID_KEYBOARD),  // android system ble can show keyboard

    0x03,   // Length
    BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID,  // ios system ble can show name GOODIX_DY
    LO_U16(BLE_ATT_SVC_HID),
    HI_U16(BLE_ATT_SVC_HID),
};


static ble_gap_adv_param_t      s_gap_adv_param;            /**< Advertising parameters for legay advertising. */
static ble_gap_adv_time_param_t s_gap_adv_time_param;       /**< Advertising time parameter. */

static const uint8_t        s_ancs_service_uuid[] = {0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4, 0x99,0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79}; //apple ancs
static bool                 s_peer_os_is_ios;
static uint8_t              s_key_id;
static app_key_click_type_t s_key_click_type;
static volatile uint8_t     s_hid_send_start;
/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */


/**
 *****************************************************************************************
 * @brief Initialize BLE security configration.
 *****************************************************************************************
 */
static void app_sec_init(void)
{
    sdk_err_t    error_code;
    ble_gap_pair_enable(true);
    ble_sec_param_t sec_param =
    {
        .level     = BLE_SEC_MODE1_LEVEL2,
        .io_cap    = BLE_SEC_IO_NO_INPUT_NO_OUTPUT,
        .oob       = false,
        .auth      = BLE_SEC_AUTH_BOND ,
        .key_size  = 16,
        .ikey_dist = BLE_SEC_KDIST_ENCKEY | BLE_SEC_KDIST_IDKEY,
        .rkey_dist = BLE_SEC_KDIST_ENCKEY | BLE_SEC_KDIST_IDKEY,
    };
    error_code = ble_sec_params_set(&sec_param);
    APP_ERROR_CHECK(error_code);

    error_code = ble_gap_privacy_params_set(150, true);
    APP_ERROR_CHECK(error_code);
}

/**
 *****************************************************************************************
 * @brief Initialize ble_advertising module.
 *****************************************************************************************
 */
static void app_adv_init(void)
{
    sdk_err_t   error_code;

    error_code = ble_gap_device_name_set(BLE_GAP_WRITE_PERM_DISABLE, DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(error_code);

    s_gap_adv_param.adv_intv_max  = ADV_FAST_MAX_INTERVAL;
    s_gap_adv_param.adv_intv_min  = ADV_FAST_MIN_INTERVAL;
    s_gap_adv_param.adv_mode      = BLE_GAP_ADV_TYPE_ADV_IND;
    s_gap_adv_param.chnl_map      = BLE_GAP_ADV_CHANNEL_37_38_39;
    s_gap_adv_param.disc_mode     = BLE_GAP_DISC_MODE_GEN_DISCOVERABLE;
    s_gap_adv_param.filter_pol    = BLE_GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;

    error_code = ble_gap_adv_param_set(0, BLE_GAP_OWN_ADDR_STATIC, &s_gap_adv_param);
    APP_ERROR_CHECK(error_code);

    error_code = ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_DATA, s_adv_data_set, sizeof(s_adv_data_set));
    APP_ERROR_CHECK(error_code);

    s_gap_adv_time_param.duration     = 0;
    s_gap_adv_time_param.max_adv_evt  = 0;
}



/**
 *****************************************************************************************
 * @brief Start advertising.
 *****************************************************************************************
 */
static void app_adv_start(void)
{
    sdk_err_t         error_code;
    error_code = ble_gap_adv_start(0, &s_gap_adv_time_param);
    APP_ERROR_CHECK(error_code);
}

/**
 *****************************************************************************************
 * @brief Print device mac address.
 *****************************************************************************************
 */
static void dev_mac_info_print(void)
{
    sdk_err_t         error_code;
    ble_gap_bdaddr_t  bd_addr;

    error_code = ble_gap_addr_get(&bd_addr);
    APP_ERROR_CHECK(error_code);
    APP_LOG_INFO("Local Board %02X:%02X:%02X:%02X:%02X:%02X.",
                 bd_addr.gap_addr.addr[5],
                 bd_addr.gap_addr.addr[4],
                 bd_addr.gap_addr.addr[3],
                 bd_addr.gap_addr.addr[2],
                 bd_addr.gap_addr.addr[1],
                 bd_addr.gap_addr.addr[0]);
}


/**
 *****************************************************************************************
 * @brief Initialize services.
 *****************************************************************************************
 */
static void services_init(void)
{
    user_mouse_service_init();
}

static void app_paring_succeed_handler(uint8_t conn_idx)
{
    ble_uuid_t ble_ancs_uuid = 
    {
        .uuid_len = BLE_ATT_UUID_128_LEN,
        .uuid = (uint8_t*)s_ancs_service_uuid,
    };
    //browse ancs services,check current phone is android or ios.
    ble_gattc_services_browse(conn_idx, &ble_ancs_uuid);
}

static void app_sec_rcv_enc_req_handler(uint8_t conn_idx, const ble_sec_evt_enc_req_t *p_enc_req)
{
    ble_sec_cfm_enc_t cfm_enc;

    if (NULL == p_enc_req)
    {
        return;
    }
    memset((uint8_t *)&cfm_enc, 0, sizeof(ble_sec_cfm_enc_t));

    switch (p_enc_req->req_type)
    {
        case BLE_SEC_PAIR_REQ:
            cfm_enc.req_type = BLE_SEC_PAIR_REQ;
            cfm_enc.accept   = true;
            break;
        default:
            break;
    }
    ble_sec_enc_cfm(conn_idx, &cfm_enc);
}

void app_disconnected_handler(uint8_t conn_idx, uint8_t reason)
{
    APP_LOG_INFO("Disconnected (0x%02X).", reason);
    s_peer_os_is_ios = false;
    app_adv_start();
}

void app_connected_handler(uint8_t conn_idx, const ble_gap_evt_connected_t *p_param)
{
    APP_LOG_INFO("Connect Peer Addr: %02X:%02X:%02X:%02X:%02X:%02X.",
                p_param->peer_addr.addr[5],
                p_param->peer_addr.addr[4],
                p_param->peer_addr.addr[3],
                p_param->peer_addr.addr[2],
                p_param->peer_addr.addr[1],
                p_param->peer_addr.addr[0]);

    // update interval to 15ms 
    ble_gap_conn_update_param_t gap_conn_param;
    gap_conn_param.interval_min  = MIN_CONN_INTERVAL;
    gap_conn_param.interval_max  = MAX_CONN_INTERVAL;
    gap_conn_param.slave_latency = SLAVE_LATENCY;
    gap_conn_param.sup_timeout   = CONN_SUP_TIMEOUT;
    ble_gap_conn_param_update(conn_idx,&gap_conn_param);
}
/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void ble_evt_handler(const ble_evt_t *p_evt)
{
    switch(p_evt->evt_id)
    {
        case BLE_COMMON_EVT_STACK_INIT:
            {
                ble_app_init();
            }
            break;

        case BLE_GAPC_EVT_CONN_PARAM_UPDATE_REQ:
            {
                ble_gap_conn_param_update_reply(p_evt->evt.gapc_evt.index, true);
            }
            break;

        case BLE_SEC_EVT_LINK_ENC_REQUEST:
            {
                app_sec_rcv_enc_req_handler(p_evt->evt.sec_evt.index, &(p_evt->evt.sec_evt.params.enc_req));
            }
            break;

        case BLE_SEC_EVT_LINK_ENCRYPTED:
            if (BLE_SUCCESS == p_evt->evt_status)
            {
                APP_LOG_INFO("Link has been successfully encrypted.");
                app_paring_succeed_handler(p_evt->evt.sec_evt.index);
            }
            else
            {
                APP_LOG_INFO("Pairing failed for error 0x%x.", p_evt->evt_status);
            }
            break;

        case BLE_GATTC_EVT_SRVC_BROWSE:
            if(p_evt->evt_status == BLE_SUCCESS)
            {
                s_peer_os_is_ios = true;
            }
            break;
            
        case BLE_GAPC_EVT_CONNECTED:
            {
                app_connected_handler(p_evt->evt.gapc_evt.index,&p_evt->evt.gapc_evt.params.connected);
            }
            break;
            
        case BLE_GAPC_EVT_DISCONNECTED:
            {
                app_disconnected_handler(p_evt->evt.gapc_evt.index,p_evt->evt.gapc_evt.params.disconnected.reason);
            }
            break;
    }
}

void ble_app_init(void)
{
    sdk_version_t     version;

    sys_sdk_verison_get(&version);
    APP_LOG_INFO("Goodix BLE SDK V%d.%d.%d (commit %x)",
                version.major, version.minor, version.build, version.commit_id);
    APP_LOG_INFO("HID DouYin example started.");
    dev_mac_info_print();
    services_init();

    app_adv_init();
    app_sec_init();
    app_adv_start();
}

 void app_key_evt_handler(uint8_t key_id, app_key_click_type_t key_click_type)
{
    s_key_id = key_id;
    s_key_click_type = key_click_type;
    s_hid_send_start = true;
    APP_LOG_INFO("Start! \n");
}

void hid_key_send_schedule(void)
{
    if (!s_hid_send_start)
    {
        return;
    }
    if (s_key_click_type == APP_KEY_SINGLE_CLICK)
    {
        switch (s_key_id)
        {
            case BSP_KEY_UP_ID:
                APP_LOG_INFO("Up key Clicked! \n");
                touch_up(s_peer_os_is_ios);
                break;

            case BSP_KEY_DOWN_ID:
                APP_LOG_INFO("Down key Clicked! \n");
                touch_down(s_peer_os_is_ios);
                break;

            default:
                break;
        }
    }
    else if (s_key_click_type == APP_KEY_DOUBLE_CLICK)
    {
        touch_double(s_peer_os_is_ios);
    }

    s_hid_send_start = false;
}

