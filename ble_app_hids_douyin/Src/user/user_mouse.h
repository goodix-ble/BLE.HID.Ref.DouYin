/**
 *****************************************************************************************
 *
 * @file user_keyboard.c
 *
 * @brief  User keyboard Implementation.
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
#ifndef _USER_MOUSE_H_
#define _USER_MOUSE_H_

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <stdbool.h>
#include <stdint.h>
#include "grx_sys.h"

/*
 * Typedefs
 *****************************************************************************************
 */
 
 
#define MIN_CONN_INTERVAL                   12                 /**< Minimum acceptable connection interval (in units of 1.25 ms). */
#define MAX_CONN_INTERVAL                   12                 /**< Maximum acceptable connection interval (in units of 1.25 ms). */
#define SLAVE_LATENCY                       0                   /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    400                 /**< Connection supervisory timeout (4 seconds). */

#define DELAY_FOR_EVERY_ACTION              15                 /**< Delay between every action ,15 = MIN_CONN_INTERVAL*1.25       */
 
/**@brief Mouse input data define. */
typedef struct
{
    uint8_t   volume_decrement:1;
    uint8_t   volume_increment:1;
    uint16_t  key_data:9;
    uint16_t  other_data:13;
}__attribute__((packed)) keyboard_data_t;

typedef struct
{
    uint8_t   tip_switch:1;
    uint8_t   in_range:1;
    uint8_t   other_data:6;
    uint8_t   contact_id;
    uint16_t  x_data;
    uint16_t  y_data;
    uint8_t   contact_cnt;
}__attribute__((packed)) finger_data_t;

typedef struct
{
    uint8_t left_button:1;
    uint8_t middle_button:1;
    uint8_t right_button:1;
    uint8_t other1_button:1;
    uint8_t other2_button:1;
    uint8_t other_data:3;
    int8_t  wheel_delta;
    int8_t  ac_delta;
}__attribute__((packed)) mouse_data_t;
typedef struct
{
    int16_t x:12;
    int16_t y:12;
}__attribute__((packed)) mouse_xy_data_t;

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Mouse used service init.
 *****************************************************************************************
 */
void user_mouse_service_init(void);
void touch_up(bool is_andorid);
void touch_down(bool is_andorid);
void touch_double(bool is_ios);
void touch_right(bool is_andorid);
void touch_left(bool is_andorid);
void touch_func(bool is_andorid);
void touch_camera(void);

#endif

