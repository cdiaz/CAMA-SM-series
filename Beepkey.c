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
                ºÏ≤‚∞¥º¸◊¥Ã¨
------------------------------------------------------------------------------*/
void ChkKey(void)
{
    static BYTE bKeyChkTimes = 0;
    static WORD wKeyClearTimes = 0;
//    static BYTE bKeyWakeypTimes = 0;
    
    BYTE bKey;
    
    if(boTime1ms)
    {
        bKey = PINB & 0x3f;
        
        if(bKey != bKeyData)
        {
            if(bKey == bKeyUpData)
            {
                bKeyChkTimes ++;
                if(bKeyChkTimes >= 10)
                {
                    bKeyChkTimes = 0;
                    bKeyData = bKey;
                    if(bKeyData != 0x3f)
                    {
                        boBeepKey = 1;
                        boKeyOk = 1;
                    }
                }
            }
            else
            {
                bKeyChkTimes = 0;
                bKeyUpData = bKey;
            }
        }
        
        if(!KeyC)
        {
            wKeyClearTimes ++;
            if(wKeyClearTimes >= 1000)
            {
                wKeyClearTimes = 0;
                if(!boKeyClearOk)
                {
                    b5sSleep = 0;
                    boKeyClearOk = 1;
                    StrCls(bAdmin,8,'1');
                    StrcpyToE2prom(bAdminE2,bAdmin,8);
                    boBeepSuccess = 1;
                }
            }
        }
        else
        {
            wKeyClearTimes = 0;
            boKeyClearOk = 0;
        }
        
/*        if(!KeyW)
        {
            bKeyWakeypTimes ++;
            if(bKeyWakeypTimes >= 10)
            {
                bKeyWakeypTimes = 0;
                boKeyWakeupOk = 1;
            }
        }
        else
        {
            bKeyWakeypTimes = 0;
            boKeyWakeupOk = 0;
        }*/
        
        boTime1ms = 0;
    }
}

/*------------------------------------------------------------------------------
                ∞¥º¸÷µº∆À„
------------------------------------------------------------------------------*/
void GetKey(void)
{
    if(boKeyOk)
    {
        boKeyOk = 0;
        b5sSleep = 0;
        if(!(bKeyData & 0x01))
        {
            bKeyCode[bKeyCodeNum ++] = Code0;
        }
        else if(!(bKeyData & 0x02))
        {
          bKeyCode[bKeyCodeNum ++] = Code2;
        }
        else if(!(bKeyData & 0x04))
        {
          bKeyCode[bKeyCodeNum ++] = Code4;
        }
        else if(!(bKeyData & 0x08))
        {
          bKeyCode[bKeyCodeNum ++] = Code1;
        }
        else if(!(bKeyData & 0x10))
        {
          bKeyCode[bKeyCodeNum ++] = Code3;
        }
        else if(!(bKeyData & 0x20))
        {
          bKeyCode[bKeyCodeNum ++] = Code5;
        }
        
        if((bKeyCodeNum == 1) || 
           ((bKeyCode[1] == Code0) && ((bKeyCodeNum == 11) || (bKeyCodeNum == 15))) || 
           ((bKeyCode[1] == Code1) && (bKeyCodeNum == 11)))
        {
            boSendFPCancelPacket = 1;
        }
    }
}


/*------------------------------------------------------------------------------
                ∞¥º¸“Ù
------------------------------------------------------------------------------*/
void BeepKey(void)
{
    if((!boBeep) && boBeepKey)
    {
        bShenDiaoHigh = Time1High6;
        bShenDiaoLow = Time1Low6;
        boBeepKey = 0;
        wNoBeepTime = 0;
        wBeepTime = 40;
        boBeep = 1;
    }
}

/*------------------------------------------------------------------------------
                ¥ÌŒÛ“Ù
------------------------------------------------------------------------------*/
void BeepError(void)
{
    static BYTE bBeepThree = 3;
    
    
    if((!boBeep) && boBeepError)
    {
        bShenDiaoHigh = Time1High;
        bShenDiaoLow = Time1Low;
        
        bBeepThree --;
        wNoBeepTime = 100;
        wBeepTime = 40;
        if(bBeepThree == 2)
        {
            wNoBeepTime = BeepToBeepTime;
        }
        else if(!bBeepThree)
        {
            bBeepThree = 3;
            boBeepError = 0;
        }
        boBeep = 1;
        
        bErrorNum ++;
        if(bErrorNum >= 9)
        {
            bErrorNum = 0;
            boSleepSys = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                ≥…π¶“Ù
------------------------------------------------------------------------------*/
void BeepSuccess(void)
{
    if((!boBeep) && boBeepSuccess)
    {
        bShenDiaoHigh = Time1High;
        bShenDiaoLow = Time1Low;
        
        boBeepSuccess = 0;
        wNoBeepTime = BeepToBeepTime;
        wBeepTime = 500;
        boBeep = 1;
        
        bErrorNum = 0;
    }
}

/*------------------------------------------------------------------------------
                µÕ—π“Ù
------------------------------------------------------------------------------*/
void BeepLowpow(void)
{
    static BYTE bBeepFive = 5;
    
    if((!boBeep) && boBeepLowpow)
    {
        bShenDiaoHigh = Time1High6;
        bShenDiaoLow = Time1Low6;
        bBeepFive --;
        wNoBeepTime = 100;
        wBeepTime = 40;
        if(!bBeepFive)
        {
            bBeepFive = 5;
            boBeepLowpow = 0;
        }
        boBeep = 1;
    }
}
