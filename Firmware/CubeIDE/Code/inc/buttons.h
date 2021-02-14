/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: buttons.h
*/

#define BTN_NO_ACTION        (0)

#define BTN_UP               (1)     //PA1
#define BTN_UP_LONG          (2)

#define BTN_DOWN             (3)     //PA2
#define BTN_DOWN_LONG        (4)

#define BTN_OK               (5)     //PA3
#define BTN_OK_LONG          (6)

#define BTN_PWR              (7)     //PA4
#define BTN_PWR_LONG         (8)

#define BTN_ESC              (9)     //PA5
#define BTN_ESC_LONG         (10)



uint8_t scan_buttons(void);
