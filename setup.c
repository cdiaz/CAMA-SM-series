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

/*------------------------------------------------------------------------------
                功能设置 校对母码
------------------------------------------------------------------------------*/
void ChkAdmin(void)
{
    if((bKeyCode[0] == Code5) && (bKeyCodeNum >= 10) && (!boChkAdminOk) && (!boSciTraStatus))
    {
        if(StrComp(bAdmin,bKeyCode + 2,8))
        {
            boAddAppoint = 0;
            boChkAdminOk = 1;
            
            if(bKeyCode[1] == Code1)
            {
                boSendIdentifyPacket = 1;
            }
            else if(bKeyCode[1] == Code0)
            {
                boSendEnrollPacket = 1;
                bPasswordNowIndex = GetEmptyRecord();
            }
        }
        else                                        //母码错误
        {
            bKeyCodeNum = 0;
            boChkAdminOk = 0;
            boBeepError = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                功能设置 恢复出厂值
------------------------------------------------------------------------------*/
void FactorySet(void)
{
    if((bKeyCode[1] == Code4) && (bKeyCodeNum >= 11) && boChkAdminOk && (!boSciTraStatus))
    {
        bKeyCodeNum = 0;
        boChkAdminOk = 0;
        
        if(bKeyCode[10] == Code5)
        {
            bFirstInitE2 = 0xff;
            E2promInit();
            boSciRecOk = 0;
            SendClearAllTemplatePacket();
        }
        else
        {
            boBeepError = 1;
        }
    }
    if(boSciRecOk && (bSciRecCtrl[0] == 0x06))
    {
        boSciRecOk = 0;
        if(!bSciRecCtrl[2])
        {
            boBeepSuccess = 1;
//            boDatabaseEmpty = 1;
            boSleepSys = 1;
        }
        else
        {
            boBeepError = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                功能设置 切换通道模式
------------------------------------------------------------------------------*/
void AllaywayMode(void)
{
    if((bKeyCode[1] == Code3) && (bKeyCodeNum >= 11) && boChkAdminOk)
    {
        bKeyCodeNum = 0;
        boChkAdminOk = 0;
        
        if(bKeyCode[10] == Code5)
        {
            
            boAllaywayModeE2 ^= 1;
            
            boBeepSuccess = 1;
            boSleepSys = 1;
        }
        else
        {
            boBeepError = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                功能设置 修改密码
------------------------------------------------------------------------------*/
void ChangePassword(void)
{
    WORD wTemp_ChangePassword;
    
    if((bKeyCode[1] == Code2) && (bKeyCodeNum >= 31) && boChkAdminOk)
    {
        bKeyCodeNum = 0;
        boChkAdminOk = 0;
        
        if((bKeyCode[30] == Code5) && StrChk(bKeyCode + 10,12) && StrComp(bKeyCode + 14,bKeyCode + 22,8))
        {
            wTemp_ChangePassword = ((bKeyCode[10] - 0x30) * 64 + (bKeyCode[11] - 0x30) * 16 + 
                                    (bKeyCode[12] - 0x30) * 4 + (bKeyCode[13] - 0x30)) * 8;
            if(!wTemp_ChangePassword)
            {
                bAdmin[0] = bKeyCode[14];
                bAdmin[1] = bKeyCode[15];
                bAdmin[2] = bKeyCode[16];
                bAdmin[3] = bKeyCode[17];
                bAdmin[4] = bKeyCode[18];
                bAdmin[5] = bKeyCode[19];
                bAdmin[6] = bKeyCode[20];
                bAdmin[7] = bKeyCode[21];
                StrcpyToE2prom(bAdminE2,bKeyCode + 14,8);
                
                boBeepSuccess = 1;
                boSleepSys = 1;
            }
            else
            {
                wTemp_ChangePassword += PasswordI2cAdr;
                wTemp_ChangePassword -= 8;
                I2cReadPage(bI2cDataBuf,1,HighByte(wTemp_ChangePassword),LowByte(wTemp_ChangePassword));
                if((bI2cDataBuf[0] != 0xff) && (bI2cDataBuf[0] != Code5))
                {
                    I2cWritePage(bKeyCode + 14,8,HighByte(wTemp_ChangePassword),LowByte(wTemp_ChangePassword));
                    
                    boBeepSuccess = 1;
                    boSleepSys = 1;
                }
                else
                {
                    boBeepError = 1;
                }
            }
        }
        else
        {
            boBeepError = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                添加操作
------------------------------------------------------------------------------*/
void Add(void)
{
    WORD wTemp_Add;
    
    if((bKeyCode[1] == Code0) && boChkAdminOk)
    {
        if(((bKeyCodeNum >= 11) && (bKeyCode[10] == Code5)) || 
           ((bKeyCodeNum >= 15) && (bKeyCode[14] == Code5)))
        {
            bKeyCodeNum = 0;
            boChkAdminOk = 0;
            boSendEnrollPacket = 0;
            boSleepSys = 1;
            bPasswordNowIndex = GetEmptyRecord();
        }
        else if((!boSciTraStatus) && boSendEnrollPacket && (!boBeepSuccess) && (!boBeepError) && (!boBeep))
        {
            boSendEnrollPacket = 0;
            SendClearTemplatePacket(bPasswordNowIndex);
            while(boSciTraStatus);
            Delayms(100);
            boSciRecOk = 0;
            SendEnrollPacket(bPasswordNowIndex);
        }
        else if(boSciRecOk && (bSciRecCtrl[0] == 0x03))
        {
            boSciRecOk = 0;
            if(!bSciRecCtrl[2])
            {
                if((bSciRecCtrl[1] == 4) &&(bSciRecCtrl[3] == 0x84))
                {
                    boBeepKey = 1;
//                    bErrorNum = 0;
                }
                else if((bSciRecCtrl[1] == 6) &&(!bSciRecCtrl[4]))
                {
                    bI2cDataBuf[0] = '#';
                    I2cWritePage(bI2cDataBuf,1,HighByte(((WORD)(bPasswordNowIndex - 1)) * 8 + PasswordI2cAdr),
                                               LowByte(((WORD)(bPasswordNowIndex - 1)) * 8 + PasswordI2cAdr));
                    
                    boDatabaseEmpty = 0;
                    boBeepSuccess = 1;
                    bPasswordNowIndex = GetEmptyRecord();
                    if(bKeyCodeNum <= 11)
                    {
                        boSendEnrollPacket = 1;
                    }
                }
            }
            else if(bSciRecCtrl[3] == 0x21)
            {
                boBeepError = 1;
            }
            else
            {
                boBeepError = 1;
//                boSleepSys = 1;
                boSendEnrollPacket = 1;
            }
        }
        else if((bKeyCodeNum == 14) && (!boAddAppoint))
        {
            if(StrChk(bKeyCode + 10,4))
            {
                bPasswordNowIndex = (bKeyCode[10] - 0x30) * 64 + (bKeyCode[11] - 0x30) * 16 +
                                    (bKeyCode[12] - 0x30) * 4  + (bKeyCode[13] - 0x30);
                if(bPasswordNowIndex)
                {
                    I2cReadPage(bI2cDataBuf,8,HighByte(((WORD)(bPasswordNowIndex - 1)) * 8 + PasswordI2cAdr),
                                              LowByte(((WORD)(bPasswordNowIndex - 1)) * 8 + PasswordI2cAdr));
                    if(bI2cDataBuf[0] == 0xff)
                    {
                        boSendEnrollPacket = 1;
                        boAddAppoint = 1;
                    }
                    else
                    {
                        boChkAdminOk = 0;
                        boAddAppoint = 1;
                        bKeyCodeNum = 0;
                        boBeepError = 1;
                        boSendEnrollPacket = 0;
                    }
                }
                else
                {
                    boChkAdminOk = 0;
                    boAddAppoint = 1;
                    bKeyCodeNum = 0;
                    boBeepError = 1;
                    boSendEnrollPacket = 0;
                }
            }
            else
            {
                boChkAdminOk = 0;
                boAddAppoint = 1;
                bKeyCodeNum = 0;
                boBeepError = 1;
                boSendEnrollPacket = 0;
            }
        }
        else if(bKeyCodeNum >= 31)
        {
            bKeyCodeNum = 0;
            boChkAdminOk = 0;
            
            if((bKeyCode[30] == Code5) && StrChk(bKeyCode + 10,12) && StrComp(bKeyCode + 14,bKeyCode + 22,8))
            {
                wTemp_Add = ((bKeyCode[10] - 0x30) * 64 + (bKeyCode[11] - 0x30) * 16 + 
                             (bKeyCode[12] - 0x30) * 4 + (bKeyCode[13] - 0x30)) * 8;
                if(wTemp_Add)
                {
                    wTemp_Add += PasswordI2cAdr;
                    wTemp_Add -= 8;
                    I2cReadPage(bI2cDataBuf,1,HighByte(wTemp_Add),LowByte(wTemp_Add));
                    if(bI2cDataBuf[0] == 0xff)
                    {
                        I2cWritePage(bKeyCode + 14,8,HighByte(wTemp_Add),LowByte(wTemp_Add));
                        
                        boBeepSuccess = 1;
                        boSleepSys = 1;
                        return;
                    }
                    else
                    {
                        boBeepError = 1;
                    }
                }
                else
                {
                    boBeepError = 1;
                }
            }
            else
            {
                boBeepError = 1;
            }
        }
    }
}

/*------------------------------------------------------------------------------
                删除操作
------------------------------------------------------------------------------*/
void Del(void)
{
    BYTE bTemp_Del;
    
    if((bKeyCode[1] == Code1) && boChkAdminOk)
    {
        if(((bKeyCodeNum >= 11) && (bKeyCode[10] == Code5)) || 
           ((bKeyCodeNum >= 15) && (bKeyCode[14] == Code5)))
        {
            bKeyCodeNum = 0;
            boChkAdminOk = 0;
            boSendIdentifyPacket = 0;
            bPasswordNowIndex = GetEmptyRecord();
            boSleepSys = 1;
        }
        else if((!boSciTraStatus) && boSendIdentifyPacket && (!boBeepSuccess) && (!boBeepError) && (!boBeep))
        {
            boSendIdentifyPacket = 0;
            SendIdentifyPacket();
        }
        else if(boSciRecOk && (bSciRecCtrl[0] == 0x02))
        {
            boSciRecOk = 0;
            
            if(!bSciRecCtrl[2])
            {
                SendClearTemplatePacket(bSciRecCtrl[3]);
                while(boSciTraStatus);
                Delayms(100);
                boSciRecOk = 0;
                boBeepSuccess = 1;
                boSendIdentifyPacket = 1;
            }
            else if(bSciRecCtrl[3] == 0x23)
            {
                boSleepSys = 1;
            }
            else
            {
                boBeepError = 1;
                boSendIdentifyPacket = 1;
            }
        }
        else if(bKeyCodeNum >= 19)
        {
            bKeyCodeNum = 0;
            boChkAdminOk = 0;
            
            if((bKeyCode[18] == Code5) && StrChk(bKeyCode + 10,4) && StrComp(bKeyCode + 10,bKeyCode + 14,4))
            {
                bTemp_Del = (bKeyCode[10] - 0x30) * 64 + (bKeyCode[11] - 0x30) * 16 + 
                            (bKeyCode[12] - 0x30) * 4 + (bKeyCode[13] - 0x30);
                if(bTemp_Del)
                {
                    I2cReadPage(bI2cDataBuf,1,HighByte(((WORD)bTemp_Del) * 8 + PasswordI2cAdr - 8),
                                              LowByte(((WORD)bTemp_Del) * 8 + PasswordI2cAdr - 8));
                    if(bI2cDataBuf[0] != 0xff)
                    {
                        if(bI2cDataBuf[0] == Code5)
                        {
                            SendClearTemplatePacket(bTemp_Del);
                            while(boSciTraStatus);
                            Delayms(100);
                            boSciRecOk = 0;
                        }
                        
                        StrCls(bI2cDataBuf,8,0xff);
                        I2cWritePage(bI2cDataBuf,8,HighByte(((WORD)bTemp_Del) * 8 + PasswordI2cAdr - 8),
                                                   LowByte(((WORD)bTemp_Del) * 8 + PasswordI2cAdr - 8));
                        
                        bPasswordNowIndex = GetEmptyRecord();
                        boBeepSuccess = 1;
                        boSleepSys = 1;
                    }
                    else
                    {
                        boBeepError = 1;
                    }
                }
                else
                {
                    boBeepError = 1;
                }
            }
            else
            {
                boBeepError = 1;
            }
        }
    }
}

void OpenDoor(void)
{
    WORD wTemp_OpenDoor;
    
    if(((!bKeyCodeNum) || ((bKeyCode[0] != Code5) && (bKeyCodeNum >= 12))) && boSendIdentifyPacket && 
       (!boBeepSuccess) && (!boBeepError) && (!boBeep) && (!boSciTraStatus))
    {
        boSendIdentifyPacket = 0;
        SendIdentifyPacket();
    }
    else if((!bKeyCodeNum) && boSciRecOk && (bSciRecCtrl[0] == 0x02))
    {
        boSciRecOk = 0;
        
        if(!bSciRecCtrl[2])
        {
            Fin = 1;
            boOpenDoor = 1;
            boBeepSuccess = 1;
        }
        else if(bSciRecCtrl[3] == 0x23)
        {
            boSleepSys = 1;
        }
        else
        {
            boBeepError = 1;
            boSendIdentifyPacket = 1;
//            SendIdentifyPacket();
        }
    }
    else if((bKeyCode[0] != Code5) && (bKeyCodeNum >= 12))
    {
        bKeyCodeNum = 0;
        if(StrChk(bKeyCode,4))
        {
            wTemp_OpenDoor = ((bKeyCode[0] - 0x30) * 64 + (bKeyCode[1] - 0x30) * 16 + 
                              (bKeyCode[2] - 0x30) * 4  + (bKeyCode[3] - 0x30)) * 8;
            if(wTemp_OpenDoor)
            {
                wTemp_OpenDoor += PasswordI2cAdr;
                wTemp_OpenDoor -= 8;
                I2cReadPage(bI2cDataBuf,8,HighByte(wTemp_OpenDoor),LowByte(wTemp_OpenDoor));
                
                if(StrComp(bKeyCode + 4,bI2cDataBuf,8))
                {
                    Fin = 1;
                    boOpenDoor = 1;
                    boBeepSuccess = 1;
                }
                else
                {
                    boBeepError = 1;
                    boSendIdentifyPacket = 1;
//                    SendIdentifyPacket();
                }
            }
            else
            {
                boBeepError = 1;
                boSendIdentifyPacket = 1;
//                SendIdentifyPacket();
            }
        }
        else
        {
            boBeepError = 1;
            boSendIdentifyPacket = 1;
//            SendIdentifyPacket();
        }
   }
}