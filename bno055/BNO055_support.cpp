/*
 ***************************************************************************
 *
 *  BNO055_support.cpp - part of sample SW for using BNO055 with Arduino
 * 
 * Usage:        BNO055 Sensor Driver Support Source File
 * 
 * (C) All rights reserved by ROBERT BOSCH GMBH
 *
 * Copyright (C) 2014 Bosch Sensortec GmbH
 *
 *	This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/
/*	Date: 2014/01/07
 *	 Revision: 1.2
 *
 */
 
#include "BNO055_support.h"

/*****************************************************************************
 * Description: *//**\brief
 *        This function initialises the structure pointer, receives
 *        and assigns the I2C address.
 *
 *
 *
 *
 *
 *  \param  bno055_t *bno055 structure pointer.
 *    
 *
 *
 *  \return communication results.
 *
 *
 ****************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ****************************************************************************/
BNO055_RETURN_FUNCTION_TYPE BNO_Init(struct bno055_t *bno055)
{

	BNO055_RETURN_FUNCTION_TYPE comres = BNO055_Zero_U8X;
	//Link the function pointers for communication (late-binding)
	bno055->bus_read = BNO055_I2C_bus_read;
	bno055->bus_write = BNO055_I2C_bus_write;
	bno055->delay_msec = _delay;
	//Initialization from the BNO055 API
	comres = bno055_init(bno055);
	return comres;

}




/*****************************************************************************
 * Description: *//**\brief
 *        This function is called when data has to be read over the I2C bus
 *
 *
 *
 *
 *
 *  \param  unsigned char dev_addr holds the device address
 *			unsigned char reg_addr holds the register address
 *			unsigned char *reg_data holds the pointer for the start of the
 *				data structure
 *			unsigned char cnt holds the count of the number of bytes to be
 *				read
 *
 *
 *  \return communication results.
 *
 *
 ****************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ****************************************************************************/
BNO055_RETURN_FUNCTION_TYPE BNO055_I2C_bus_read(unsigned char dev_addr,unsigned char reg_addr, unsigned char *reg_data, unsigned char cnt)
{
	BNO055_RETURN_FUNCTION_TYPE comres = BNO055_Zero_U8X;
	if(auto &&wrt = Wire.get_writer(dev_addr)){
		wrt << reg_addr;
	}else{
		Serial.println("Error[Cannot write on BNO055_I2C_bus_read]");
	}
	delayMicroseconds(200);
	if(auto &&rdr = Wire.get_reader(dev_addr,cnt)){
		for(uint8 i =0;i<cnt;i++){
			rdr >> *reg_data;
			reg_data++;
		}
	delayMicroseconds(500);
	}else{
		Serial.println("Error[Cannot read on BNO055_I2C_bus_read]");
	}
	return comres;
}





/*****************************************************************************
 * Description: *//**\brief
 *        This function is called when data has to be written over
 *			I2C bus
 *
 *
 *
 *
 *
 *  \param  unsigned char dev_addr holds the device address
 *			unsigned char reg_addr holds the register address
 *			unsigned char *reg_data holds the pointer for the start of the
 *				data structure
 *			unsigned char cnt holds the count of the number of bytes to be
 *				written
 *
 *
 *  \return communication results.
 *
 *
 ****************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ****************************************************************************/
BNO055_RETURN_FUNCTION_TYPE BNO055_I2C_bus_write(unsigned char dev_addr,unsigned char reg_addr, unsigned char *reg_data, unsigned char cnt)
{
	BNO055_RETURN_FUNCTION_TYPE comres = BNO055_Zero_U8X;
	if(auto &&wrt = Wire.get_writer(dev_addr)){
		wrt << reg_addr;
		for(uint8 i=0;i<cnt;i++){
			wrt << *reg_data;
			reg_data++;
		}
	}else{
		Serial.println("Error[Cannot write on BNO055_I2C_bus_write]");
	}
	delayMicroseconds(100);
	return comres;
}

/*****************************************************************************
 * Description: *//**\brief
 *        This function is a mirror for the delay function for type casting
 *
 *
 *
 *
 *
 *  \param  unsigned int
 *
 *
 *  \return none
 *
 *
 ****************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks: 
 *
 ****************************************************************************/
void _delay(unsigned int period)
{
	delay(long(period));
}
