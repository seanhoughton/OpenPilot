/*
 * mcp9804.c
 *
 *  Created on: Nov 1, 2011
 *      Author: scott
 */

//#include "mcp9804.h"

/**
* Read the cold junction temperature from MCP9804 via I2C
* Returns true if successful and false if not.
*/

#include "openpilot.h"

bool MCP9804_ReadColdJunctionTemp(double_t* pColdTemp, uint16_t I2CAddress)
{
	uint8_t setToReadAmbientTempRegister = 0x05;
	uint8_t coldBuff[2] = {0};

	const struct pios_i2c_txn txn_list_1[] = {
		{
		 .addr = I2CAddress,// & 0xFE, //Bit 0 must be 0 to write
		 .rw = PIOS_I2C_TXN_WRITE,
		 .len = 1,
		 .buf = &setToReadAmbientTempRegister, //Read ambient temperature register
		 },
		{
		 .addr = I2CAddress,// | 0x01, //Bit 0 must be 1 to read
		 .rw = PIOS_I2C_TXN_READ,
		 .len = 2,
		 .buf = coldBuff,
		 },

	};


	if( PIOS_I2C_Transfer(PIOS_I2C_MAIN_ADAPTER, txn_list_1, NELEMENTS(txn_list_1))) {
		//Convert the temperature data
		//First Check flag bits
		if((coldBuff[0] & 0x80) == 0x80) { //TA ≥ TCRIT
		}

		if((coldBuff[0] & 0x40) == 0x40) { //TA > TUPPER
		}

		if((coldBuff[0] & 0x20) == 0x20) { //TA < TLOWER
		}

		coldBuff[0] = coldBuff[0] & 0x1F; //Clear flag bits

		double_t coldMSB = coldBuff[0];
		double_t coldLSB = coldBuff[1];
		if((coldBuff[0] & 0x10) == 0x10) { //TA < 0°C
			coldBuff[0] = coldBuff[0] & 0x0F; //Clear SIGN

			*pColdTemp = 256 - (coldMSB * 16 + coldLSB / 16.0); //2's complement
		}
		else //TA  ≥ 0°C
			*pColdTemp = coldMSB * 16 + coldLSB / 16.0;

		return true;
	}
	else
		return false;
}

