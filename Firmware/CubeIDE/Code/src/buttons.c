/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: buttons.c
*/

#include "stm32f10x.h"
#include "buttons.h"



//SCAN_INTERVAL: 1000000 = 0.8 s; 1500 = 1 ms
#define SCAN_INTERVAL           (1500)

#define BUTTONS_NUM             (5)     //five buttons total

#define BUTTON_WEIGHT_MAX       (16)
#define BUTTON_WEIGHT_MIN       (0)

#define BUTTON_THRESHOLD_HI     (12)    //0.8 of BUTTON_WEIGHT_MAX
#define BUTTON_THRESHOLD_LO     (4)     //0.2 of BUTTON_WEIGHT_MAX

#define BUTTON_PRESSED          (0)
#define BUTTON_RELEASED         (1)

#define BUTTON_PRESSED_COUNTER_MAX          (UINT16_MAX)
#define BUTTON_PRESSED_COUNTER_THRESHOLD    (350)       //short or long cick?

#define TIMEOUT_NO_OVERFLOW     (0)
#define TIMEOUT_OVERFLOW        (1)

#define BUTTON_ACTIONS_NUM      (2)     //two actions: short click, long click
#define BUTTON_ACTION_SHORT     (0)
#define BUTTON_ACTION_LONG      (1)



uint32_t scan_interval_counter = 0;
uint32_t idr_register_copy = 0;

uint8_t button_weight[BUTTONS_NUM] = {BUTTON_WEIGHT_MAX,
                                      BUTTON_WEIGHT_MAX,
                                      BUTTON_WEIGHT_MAX,
                                      BUTTON_WEIGHT_MAX,
                                      BUTTON_WEIGHT_MAX};

uint8_t button_state[BUTTONS_NUM]  = {BUTTON_RELEASED,
                                      BUTTON_RELEASED,
                                      BUTTON_RELEASED,
                                      BUTTON_RELEASED,
                                      BUTTON_RELEASED};

uint8_t button_prev_state[BUTTONS_NUM]   = {BUTTON_RELEASED,
                                            BUTTON_RELEASED,
                                            BUTTON_RELEASED,
                                            BUTTON_RELEASED,
                                            BUTTON_RELEASED};

uint16_t button_pressed_counter[BUTTONS_NUM] = {0};

uint8_t timeout_state[BUTTONS_NUM]  = {TIMEOUT_NO_OVERFLOW,
                                       TIMEOUT_NO_OVERFLOW,
                                       TIMEOUT_NO_OVERFLOW,
                                       TIMEOUT_NO_OVERFLOW,
                                       TIMEOUT_NO_OVERFLOW};

uint8_t timeout_prev_state[BUTTONS_NUM]   = {TIMEOUT_NO_OVERFLOW,
                                             TIMEOUT_NO_OVERFLOW,
                                             TIMEOUT_NO_OVERFLOW,
                                             TIMEOUT_NO_OVERFLOW,
                                             TIMEOUT_NO_OVERFLOW};



//button return code = {ButtonNumber(0...BUTTONS_NUM-1) * BUTTON_ACTIONS_NUM + BUTTON_ACTION(_SHORT)(_LONG)} + 1
uint8_t scan_buttons(void)
{
    if (scan_interval_counter < SCAN_INTERVAL)
    {
        scan_interval_counter++;            //increase counter
    }
    else
    {
        scan_interval_counter = 0;          //time to scan buttons! reset counter
        
        idr_register_copy = GPIOA->IDR;             //save pins data
        idr_register_copy &= (GPIO_IDR_IDR1 |       //mask needed pins
                              GPIO_IDR_IDR2 |
                              GPIO_IDR_IDR3 |
                              GPIO_IDR_IDR4 |
                              GPIO_IDR_IDR5);
        
        idr_register_copy >>= 1;            //align bits to the left, now IDR1(PA1) is in LSBit position
        
        for (uint8_t i = 0; i < BUTTONS_NUM; i++)       //update buttons weight (digital capacitor, increase or decrease "charge")
        {
            if (idr_register_copy & ((uint32_t)1 << i)) //check pin state
            {
                if (button_weight[i] < BUTTON_WEIGHT_MAX)
                {
                    button_weight[i] += 1;      // if input pin = 1, increase weight
                }
            }
            else
            {
                if (button_weight[i] > BUTTON_WEIGHT_MIN)
                {
                    button_weight[i] -= 1;      // if input pin = 0, decrease weight
                }
            }
        }
        
        for (uint8_t i = 0; i < BUTTONS_NUM; i++)       //make a desicion "button is pressed or not?" (with hysteresis)
        {
            if (button_weight[i] > BUTTON_THRESHOLD_HI)
            {
                button_prev_state[i] = button_state[i];
                button_state[i] = BUTTON_RELEASED;
            }
            else if (button_weight[i] < BUTTON_THRESHOLD_LO)
            {
                button_prev_state[i] = button_state[i];
                button_state[i] = BUTTON_PRESSED;
            }
        }
        
        for (uint8_t i = 0; i < BUTTONS_NUM; i++)       //check click duration. short click after button released (and before timeout overflow); long click after timeout overflow
        {
            if ((button_state[i] == BUTTON_PRESSED) && (button_pressed_counter[i] < BUTTON_PRESSED_COUNTER_MAX))   //increase timeout counter, update overflow states
            {
                button_pressed_counter[i]++;
                
                timeout_prev_state[i] = timeout_state[i];
                timeout_state[i] = (button_pressed_counter[i] > BUTTON_PRESSED_COUNTER_THRESHOLD);     //overflow or not? 1 or 0?
            }
            
            if ((button_state[i] == BUTTON_RELEASED) && (button_prev_state[i] == BUTTON_PRESSED))   //button has been released
            {
                button_pressed_counter[i] = 0;
                
                if (timeout_state[i] == TIMEOUT_NO_OVERFLOW)
                {
                    return (i * BUTTON_ACTIONS_NUM + BUTTON_ACTION_SHORT) + 1;              //if before overflow, then short click
                }
            }
            
            if ((timeout_state[i] == TIMEOUT_OVERFLOW) && (timeout_prev_state[i] == TIMEOUT_NO_OVERFLOW))   //overflow occured
            {
                return (i * BUTTON_ACTIONS_NUM + BUTTON_ACTION_LONG) + 1;   //long click
            }
        }
    }
    return BTN_NO_ACTION;
}
