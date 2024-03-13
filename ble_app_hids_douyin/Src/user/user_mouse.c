/**
 *****************************************************************************************
 *
 * @file user_mouse.c
 *
 * @brief The implementation of BLE Mouse functions.
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
#include "user_mouse.h"
#include "bas.h"
#include "dis.h"
#include "hids.h"
#include "app_log.h"
#include "app_error.h"
#include "utility.h"

/*
 * DEFINES
 *******************************************************************************
 */
#define HID_KEYBOARD_REP_ID         3
#define HID_FINGER_REP_ID           2
#define HID_MOUSE_REP_ID            4
#define HID_MOUSE_XY_REP_ID         5
#define HID_COUNT_MAX_REP_ID        8

#define INPUT_REPORT_COUNT          4

#define HID_KEYBOARD_INDEX          0
#define HID_FINGER_INDEX            1
#define HID_MOUSE_INDEX             2
#define HID_MOUSE_XY_INDEX          3

#define BASE_USB_HID_SPEC_VERSION           0x0101        /**< Version number of base USB HID Specification implemented by this application. */

#define HID_X_Y_SPEED                       300
#define BLE_CONN_EVT_SKIP_CNT               2
#define BLE_SEND_DATA_INTVERL               1


/*
 * LOCAL VARIABLE DEFINITIONS
 *******************************************************************************
 */

static bool s_in_notify_enabled = false;

/*
This hid map include
Phone Volume key :  take photo and control volume
Mouse            :  control ios phone touch up,touch down,double touch
Finger           :  control android phone touch up,touch down,double touch
*/
static const uint8_t rep_map_data[] =
{
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x0B,        //   Report Count (11)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x0A, 0xAE, 0x01,  //   Usage (AL Keyboard Layout)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x0D,        //   Report Size (13)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x04,        // Usage (Touch)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x09, 0x22,        //   Usage (Finger)
    0xA1, 0x02,        //   Collection (Logical)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x32,        //     Usage (In Range)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //     Report Count (6)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //     Report Size (8)
    0x09, 0x51,        //     Usage (0x51)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x26, 0xFF, 0x0F,  //     Logical Maximum (4095)
    0x75, 0x10,        //     Report Size (16)
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x33,        //     Unit (System: English Linear, Length: Inch)
    0x09, 0x30,        //     Usage (X)
    0x35, 0x00,        //     Physical Minimum (0)
    0x46, 0xB5, 0x04,  //     Physical Maximum (1205)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x46, 0x8A, 0x03,  //     Physical Maximum (906)
    0x09, 0x31,        //     Usage (Y)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x05, 0x0D,        //   Usage Page (Digitizer)
    0x09, 0x54,        //   Usage (0x54)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x08,        //   Report ID (8)
    0x09, 0x55,        //   Usage (0x55)
    0x25, 0x05,        //   Logical Maximum (5)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x04,        //   Report ID (4)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x95, 0x05,        //     Report Count (5)
    0x75, 0x01,        //     Report Size (1)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x03,        //     Report Size (3)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0C,        //     Usage Page (Consumer)
    0x0A, 0x38, 0x02,  //     Usage (AC Pan)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x85, 0x05,        //   Report ID (5)
    0x09, 0x01,        //   Usage (Consumer Control)
    0xA1, 0x00,        //   Collection (Physical)
    0x75, 0x0C,        //     Report Size (12)
    0x95, 0x02,        //     Report Count (2)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x16, 0x01, 0xF8,  //     Logical Minimum (-2047)
    0x26, 0xFF, 0x07,  //     Logical Maximum (2047)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Process HID Service events.
 *
 * @param[in] p_evt: Pointer of HID Service event.
 *****************************************************************************************
 */
static void hid_service_event_process(hids_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case HIDS_EVT_BOOT_MODE_ENTERED:
            break;

        case HIDS_EVT_REPORT_MODE_ENTERED:
            break;

        case HIDS_EVT_IN_REP_NOTIFY_ENABLED:
            s_in_notify_enabled = true;
            break;

        case HIDS_EVT_IN_REP_NOTIFY_DISABLED:
            s_in_notify_enabled = false;
            break;

        default:
            break;
    }

}

/**
 *****************************************************************************************
 *@brief HID Service init.
 *****************************************************************************************
 */
static void user_hids_init(void)
{
    hids_init_t hids_init;
    uint8_t     hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK |
                                 HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;

    hids_init.evt_handler  = hid_service_event_process;


    hids_init.hid_info.bcd_hid        = BASE_USB_HID_SPEC_VERSION;
    hids_init.hid_info.b_country_code = 0;
    hids_init.hid_info.flags          = hid_info_flags;

    hids_init.report_map.p_map = (uint8_t*)&rep_map_data;
    hids_init.report_map.len   = sizeof(rep_map_data);

    hids_init.input_report_count                                          = INPUT_REPORT_COUNT;
    
    hids_init.input_report_array[HID_KEYBOARD_INDEX].value_len         = sizeof(keyboard_data_t);
    hids_init.input_report_array[HID_KEYBOARD_INDEX].ref.report_id     = HID_KEYBOARD_REP_ID;
    hids_init.input_report_array[HID_KEYBOARD_INDEX].ref.report_type   = HIDS_REP_TYPE_INPUT;
    
    hids_init.input_report_array[HID_FINGER_INDEX].value_len         = sizeof(finger_data_t);
    hids_init.input_report_array[HID_FINGER_INDEX].ref.report_id     = HID_FINGER_REP_ID;
    hids_init.input_report_array[HID_FINGER_INDEX].ref.report_type   = HIDS_REP_TYPE_INPUT;
    
    hids_init.input_report_array[HID_MOUSE_INDEX].value_len         = sizeof(mouse_data_t);
    hids_init.input_report_array[HID_MOUSE_INDEX].ref.report_id     = HID_MOUSE_REP_ID;
    hids_init.input_report_array[HID_MOUSE_INDEX].ref.report_type   = HIDS_REP_TYPE_INPUT;
    
    hids_init.input_report_array[HID_MOUSE_XY_INDEX].value_len         = sizeof(mouse_xy_data_t);
    hids_init.input_report_array[HID_MOUSE_XY_INDEX].ref.report_id     = HID_MOUSE_XY_REP_ID;
    hids_init.input_report_array[HID_MOUSE_XY_INDEX].ref.report_type   = HIDS_REP_TYPE_INPUT;

    uint8_t count_max = 1;
    hids_init.count_max_feat_report.value_len       = sizeof(count_max);
    hids_init.count_max_feat_report.ref.report_id   = HID_COUNT_MAX_REP_ID;
    hids_init.count_max_feat_report.ref.report_type = HIDS_REP_TYPE_FEATURE;
    memcpy(hids_init.count_max_feat, &count_max, sizeof(count_max));

    sdk_err_t  ret = hids_service_init(&hids_init);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 */
void user_mouse_service_init(void)
{
    user_hids_init();
}


//static sdk_err_t user_keyboard_data_send(uint8_t conn_idx, keyboard_data_t *p_data)
//{
//    sdk_err_t err_code;
//    if (s_in_notify_enabled)
//    {
//        err_code = hids_input_rep_send(conn_idx,HID_KEYBOARD_INDEX,(uint8_t*)p_data, sizeof(keyboard_data_t));
//    }
//    return err_code;
//}

static sdk_err_t user_finger_data_send(uint8_t conn_idx, finger_data_t *p_data)
{
    sdk_err_t err_code;
    if (s_in_notify_enabled)
    {
        err_code = hids_input_rep_send(conn_idx,HID_FINGER_INDEX,(uint8_t*)p_data, sizeof(finger_data_t));
    }
    
    return err_code;
}

static sdk_err_t user_mouse_data_send(uint8_t conn_idx, mouse_data_t *p_data)
{
    sdk_err_t err_code;
    if (s_in_notify_enabled)
    {
        err_code = hids_input_rep_send(conn_idx,HID_MOUSE_INDEX,(uint8_t*)p_data, sizeof(mouse_data_t));
    }
    return err_code;
}

static sdk_err_t user_mouse_xy_data_send(uint8_t conn_idx, mouse_xy_data_t *p_data)
{
    sdk_err_t err_code;
    if (s_in_notify_enabled)
    {
        err_code = hids_input_rep_send(conn_idx,HID_MOUSE_XY_INDEX,(uint8_t*)p_data, sizeof(mouse_xy_data_t));
    }
    return err_code;
}

void touch_up(bool is_ios)
{
    finger_data_t finger_data = {0};
    mouse_data_t  mouse_data = {0};
    mouse_xy_data_t mouse_xy_data = {0};
    APP_LOG_INFO("touch up, check iOS platform : %s",is_ios ? "true" : "false");
    if (is_ios)
    {
        mouse_xy_data.x = -2047;
        mouse_xy_data.y = 2047;
        user_mouse_xy_data_send(0, &mouse_xy_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_xy_data.x = 80;
        mouse_xy_data.y = -120;
        user_mouse_xy_data_send(0, &mouse_xy_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_data.wheel_delta = 120;
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_xy_data.x = -2047;
        mouse_xy_data.y = 2047;
        user_mouse_xy_data_send(0, &mouse_xy_data);
    }
    else
    {
        finger_data.tip_switch  = 1;
        finger_data.in_range    = 1;
        finger_data.contact_id  = 6;
        finger_data.contact_cnt = 1;
        finger_data.x_data      = 1904;
        finger_data.y_data      = 3200;
        user_finger_data_send(0, &finger_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        finger_data.y_data      = 2600;
        for (uint8_t i = 0; i < 8; i++)
        {
            user_finger_data_send(0, &finger_data);
            delay_ms(DELAY_FOR_EVERY_ACTION);
            finger_data.y_data -= HID_X_Y_SPEED;
        }
        finger_data.x_data      = 1904;
        finger_data.y_data      = 3200;
        finger_data.tip_switch  = 0;
        finger_data.in_range    = 0;
        finger_data.contact_cnt = 0;
        user_finger_data_send(0, &finger_data);
    }
}

void touch_down(bool is_ios)
{
    finger_data_t finger_data = {0};
    mouse_data_t  mouse_data = {0};
    mouse_xy_data_t mouse_xy_data = {0};
    APP_LOG_INFO("touch down, check iOS platform : %s",is_ios ? "true" : "false");
    if (is_ios)
    {
        mouse_xy_data.x = -2047;
        mouse_xy_data.y = 2047;
        user_mouse_xy_data_send(0, &mouse_xy_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_xy_data.x = 80;
        mouse_xy_data.y = -120;
        user_mouse_xy_data_send(0, &mouse_xy_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_data.wheel_delta = -120;
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_xy_data.x = -2047;
        mouse_xy_data.y = 2047;
        user_mouse_xy_data_send(0, &mouse_xy_data);
    }
    else
    {
        finger_data.tip_switch  = 1;
        finger_data.in_range    = 1;
        finger_data.contact_id  = 6;
        finger_data.contact_cnt = 1;
        finger_data.x_data      = 1904;
        finger_data.y_data      = 1012 ;
        user_finger_data_send(0, &finger_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        finger_data.y_data      = 1100;
        for (uint8_t i = 0; i < 8; i++)
        {
            user_finger_data_send(0, &finger_data);
            delay_ms(DELAY_FOR_EVERY_ACTION);
            finger_data.y_data += HID_X_Y_SPEED;
        }
        finger_data.x_data      = 1904;
        finger_data.y_data      = 1012;
        finger_data.tip_switch  = 0;
        finger_data.in_range    = 0;
        finger_data.contact_cnt = 0;
        user_finger_data_send(0, &finger_data);
    }
}

void touch_double(bool is_ios)
{
    finger_data_t finger_data = {0};
    mouse_data_t  mouse_data = {0};
    mouse_xy_data_t mouse_xy_data = {0};
    APP_LOG_INFO("touch double, check iOS platform : %s",is_ios ? "true" : "false");
    if (is_ios)
    {
        mouse_xy_data.x = -2047;
        mouse_xy_data.y = 2047;
        user_mouse_xy_data_send(0, &mouse_xy_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_xy_data.x = 120;
        mouse_xy_data.y = -360;
        user_mouse_xy_data_send(0, &mouse_xy_data);
        delay_ms(DELAY_FOR_EVERY_ACTION*6);
        mouse_data.left_button =1;
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_data.left_button =0;
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION*4);
        mouse_data.left_button =1;
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        mouse_data.left_button =0;
        user_mouse_data_send(0, &mouse_data);
        delay_ms(DELAY_FOR_EVERY_ACTION*6);
        mouse_xy_data.x = -2047;
        mouse_xy_data.y = 2047;
        user_mouse_xy_data_send(0, &mouse_xy_data);
    }
    else
    {
        finger_data.tip_switch  = 1;
        finger_data.in_range    = 1;
        finger_data.contact_id  = 6;
        finger_data.contact_cnt = 1;
        finger_data.x_data      = 1904;
        finger_data.y_data      = 2024;
        user_finger_data_send(0, &finger_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        finger_data.x_data      = 1904;
        finger_data.y_data      = 2024;
        finger_data.tip_switch  = 0;
        finger_data.in_range    = 0;
        finger_data.contact_cnt = 0;
        user_finger_data_send(0, &finger_data);
        delay_ms(DELAY_FOR_EVERY_ACTION*4);
        finger_data.tip_switch  = 1;
        finger_data.in_range    = 1;
        finger_data.contact_id  = 6;
        finger_data.contact_cnt = 1;
        finger_data.x_data      = 1904;
        finger_data.y_data      = 2024;
        user_finger_data_send(0, &finger_data);
        delay_ms(DELAY_FOR_EVERY_ACTION);
        finger_data.x_data      = 1904;
        finger_data.y_data      = 2024;
        finger_data.tip_switch  = 0;
        finger_data.in_range    = 0;
        finger_data.contact_cnt = 0;
        user_finger_data_send(0, &finger_data);
    }
}
