/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: i2c.h
*/



#ifndef I2C_HEADER
#define I2C_HEADER



void i2c_init(void);
void i2c_clock_disable(void);
void i2c_clock_enable(void);



#endif /*I2C_HEADER*/
