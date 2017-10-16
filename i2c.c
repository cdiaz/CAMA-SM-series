#include <iom16.h>
#include <stdbool.h>
#include <ina90.h>
#include <intrinsics.h>
#include "define.h"
#include "Suo.h"
#include "main.h"
#include "I2c.h"
#include "Setup.h"
#include "Scictrl.h"
#include "Beepkey.h"

void Delaynop(void)
{
	BYTE bTemp_Delaynop;
	
	bTemp_Delaynop = 4;
	while(bTemp_Delaynop --);
}

void I2cStart(void)
{
	Delaynop();
	SDARW = 1;
	SDAO = 1;
	Delaynop();
	SCL = 1;                       //start
	Delaynop();
	SDAO = 0;
	Delaynop();
	SCL = 0;
	Delaynop();
	Delaynop();
	Delaynop();
}

void I2cStop(void)
{
	Delaynop();
	SDAO = 0;
	Delaynop();
	SCL = 1;                       //stop
	Delaynop();
	SDAO = 1;
}

void I2cSendByte(BYTE bData_I2cSendByte)
{
	BYTE bTemp_I2cSendByte;
	
	for(bTemp_I2cSendByte = 0;bTemp_I2cSendByte < 8;bTemp_I2cSendByte ++)
	{
		if(bData_I2cSendByte & 0x80)
		{
			SDAO = 1;
		}            
		else
		{
			SDAO = 0;
		}
		bData_I2cSendByte <<= 1;
		Delaynop();
		SCL = 1;
		Delaynop();
		SCL = 0;
		Delaynop();
	}
	
	SDAO = 1;
	SDARW = 0;
	Delaynop();
	SCL = 1;
	Delaynop();
	Delaynop();
	Delaynop();
	SDARW = 1;
	SCL = 0;
	Delaynop();
}

/*-----------------------------------------------------------------------------
				I2c数据读子程序（页模式）
------------------------------------------------------------------------------*/
void I2cReadPage(BYTE* pData_I2cReadPage,BYTE bNumber_I2cReadPage,BYTE bI2cAdrH_I2cReadPage,BYTE bI2cAdrL_I2cReadPage)
{
	BYTE i_I2cReadPage;
	BYTE bTemp_I2cReadPage = I2cWriteLable;
//	BYTE bError_I2cReadPage = 0xff;

	I2cStart();

	I2cSendByte(I2cWriteLable | (bI2cAdrH_I2cReadPage << 1));

	I2cSendByte(bI2cAdrL_I2cReadPage);
	
	I2cStart();

	I2cSendByte(I2cReadLable);

	for(bI2cAdrH_I2cReadPage = 0;;)                                     //read a byte
	{

		SDAO = 1;
		SDARW = 0;
		
		bTemp_I2cReadPage = 0;
		for(i_I2cReadPage = 0;i_I2cReadPage < 8;i_I2cReadPage ++)
		{
			bTemp_I2cReadPage <<= 1;
			Delaynop();
			SCL = 1;
			Delaynop();
			if(SDAI)
			{
				bTemp_I2cReadPage |= 0x01;
			}
			else
			{
				bTemp_I2cReadPage &= 0xfe;
			}
			SCL = 0;
			Delaynop();
		}

		SDARW = 1;
		pData_I2cReadPage[bI2cAdrH_I2cReadPage ++] = bTemp_I2cReadPage ^ Encrypt;
		if(bI2cAdrH_I2cReadPage >= bNumber_I2cReadPage)
		{
			break;
		}
		
		Delaynop();
		SDAO = 0;
		Delaynop();
		SCL = 1;
		Delaynop();
		Delaynop();
		Delaynop();
		SCL = 0;
		Delaynop();
	}
	
	Delaynop();
	SDAO = 1;
	SDARW = 0;
	Delaynop();
	SCL = 1;
	Delaynop();
	Delaynop();
	Delaynop();
	SDARW = 1;
	SCL = 0;
	Delaynop();

	I2cStop();
}

/*-----------------------------------------------------------------------------
				I2c数据写子程序（页模式）
------------------------------------------------------------------------------*/
void I2cWritePage(BYTE* pData_I2cWritePage,BYTE bNumber_I2cWritePage,BYTE bI2cAdrH_I2cWritePage,BYTE bI2cAdrL_I2cWritePage)
{
	BYTE i_I2cWritePage;

    while(1)
    {
    	I2cStart();
    
    	I2cSendByte(I2cWriteLable | (bI2cAdrH_I2cWritePage << 1));
    	
    	I2cSendByte(bI2cAdrL_I2cWritePage);
    
    	for(i_I2cWritePage = 0;i_I2cWritePage < bNumber_I2cWritePage;i_I2cWritePage ++)
    	{
    		I2cSendByte(pData_I2cWritePage[i_I2cWritePage] ^ Encrypt);
    	}
        
    	I2cStop();
        
        Delayms(10);
        
        I2cReadPage(bI2cDataBuf + I2cBufLen / 2,bNumber_I2cWritePage,bI2cAdrH_I2cWritePage,bI2cAdrL_I2cWritePage);
    	for(i_I2cWritePage = 0;i_I2cWritePage < bNumber_I2cWritePage;i_I2cWritePage ++)
    	{
    		if(pData_I2cWritePage[i_I2cWritePage] != bI2cDataBuf[I2cBufLen / 2 + i_I2cWritePage])
            {
                break;
            }
    	}
        if(i_I2cWritePage >= bNumber_I2cWritePage)
        {
            return;
        }
    }
}