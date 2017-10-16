#ifndef _i2c_h_
#define _i2c_h_

void Delaynop(void);
void I2cStart(void);
void I2cStop(void);
void I2cSendByte(BYTE bData_I2cSendByte);
void I2cReadPage(BYTE* pData_I2cReadPage,BYTE bNumber_I2cReadPage,BYTE bI2cAdrH_I2cReadPage,BYTE bI2cAdrL_I2cReadPage);
void I2cWritePage(BYTE* pData_I2cWritePage,BYTE bNumber_I2cWritePage,BYTE bI2cAdrH_I2cWritePage,BYTE bI2cAdrL_I2cWritePage);

#endif
