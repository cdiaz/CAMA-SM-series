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
                发送指令：1:N比对
------------------------------------------------------------------------------*/
void SendIdentifyPacket(void)                                                   //1:N比对
{
    StrcpyFromFlash(bSciTraBuf,coPacket2,24);
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：指纹登记
------------------------------------------------------------------------------*/
void SendEnrollPacket(BYTE bTemplateId_SendEnrollPacket)                        //指纹登记
{
    StrcpyFromFlash(bSciTraBuf,coPacket3,24);
    bSciTraBuf[6] = bTemplateId_SendEnrollPacket;
    if(!(bTemplateId_SendEnrollPacket <= (0xff - bSciTraBuf[22])))
    {
        bSciTraBuf[23] ++;
    }
    bSciTraBuf[22] += bTemplateId_SendEnrollPacket;
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：删除模板
------------------------------------------------------------------------------*/
void SendClearTemplatePacket(BYTE bTemplateId_SendClearTemplatePacket)          //删除模板
{
    StrcpyFromFlash(bSciTraBuf,coPacket5,24);
    bSciTraBuf[6] = bTemplateId_SendClearTemplatePacket;
    if(!(bTemplateId_SendClearTemplatePacket <= (0xff - bSciTraBuf[22])))
    {
        bSciTraBuf[23] ++;
    }
    bSciTraBuf[22] += bTemplateId_SendClearTemplatePacket;
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：清空模板
------------------------------------------------------------------------------*/
void SendClearAllTemplatePacket(void)                                           //清空模板
{
    StrcpyFromFlash(bSciTraBuf,coPacket6,24);
    boSciTraStatus = 1;
    SciTraEnable();

}

/*------------------------------------------------------------------------------
                发送指令：读取指定模板状态
------------------------------------------------------------------------------*/
void SendGetTemplateStatusPacket(BYTE bTemplateId_SendGetTemplateStatusPacket)  //读取指定模板状态
{
    StrcpyFromFlash(bSciTraBuf,coPacket8,24);
    bSciTraBuf[6] = bTemplateId_SendGetTemplateStatusPacket;
    if(!(bTemplateId_SendGetTemplateStatusPacket <= (0xff - bSciTraBuf[22])))
    {
        bSciTraBuf[23] ++;
    }
    bSciTraBuf[22] += bTemplateId_SendGetTemplateStatusPacket;
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：检查所有模板
------------------------------------------------------------------------------*/
void SendGetBrokenTemplatePacket(void)                                          //检查所有模板状态
{
    StrcpyFromFlash(bSciTraBuf,coPacket9,24);
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：设置安全等级
------------------------------------------------------------------------------*/
void SendSetSecurityLevelPacket(BYTE bSecurityLevel)                            //设置安全等级
{
    StrcpyFromFlash(bSciTraBuf,coPacket12,24);
    bSciTraBuf[6] = bSecurityLevel;
    if(!(bSecurityLevel <= (0xff - bSciTraBuf[22])))
    {
        bSciTraBuf[23] ++;
    }
    bSciTraBuf[22] += bSecurityLevel;
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：设置超时时间
------------------------------------------------------------------------------*/
void SendSetFingerTimeOutPacket(BYTE bFingerTimeOut)                            //设置超时时间
{
    StrcpyFromFlash(bSciTraBuf,coPacket14,24);
    bSciTraBuf[6] = bFingerTimeOut;
    if(!(bFingerTimeOut <= (0xff - bSciTraBuf[22])))
    {
        bSciTraBuf[23] ++;
    }
    bSciTraBuf[22] += bFingerTimeOut;
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                发送指令：取消当前操作
------------------------------------------------------------------------------*/
void SendFPCancelPacket(void)                                                   //取消当前操作
{
    if((boSendFPCancelPacket) && (!boSciTraStatus))
    {
        boSendFPCancelPacket = 0;
        StrcpyFromFlash(bSciTraBuf,coPacket48,24);
        boSciTraStatus = 1;
        SciTraEnable();
    }
}

/*------------------------------------------------------------------------------
                接收数据处理
------------------------------------------------------------------------------*/
void RecCtrl(void)
{
    BYTE bTemp_RecCtrl;
    WORD wTemp_RecCtrl = 0;
    
    if(boSciRecStatus)
    {
        boSciRecStatus = 0;
        b5sSleep = 0;
        for(bTemp_RecCtrl = 0;bTemp_RecCtrl < 22;bTemp_RecCtrl ++)
        {
            wTemp_RecCtrl += bSciRecBuf[bTemp_RecCtrl];
        }
        if(((wTemp_RecCtrl % 0x100) == bSciRecBuf[22]) && ((wTemp_RecCtrl / 0x100) == bSciRecBuf[23]) &&
           (bSciRecBuf[2] != 0x30))
        {
            bSciRecCtrl[0] = bSciRecBuf[2];
            bSciRecCtrl[1] = bSciRecBuf[4];
            bSciRecCtrl[2] = bSciRecBuf[6];
            bSciRecCtrl[3] = bSciRecBuf[8];
            bSciRecCtrl[4] = bSciRecBuf[10];
            boSciRecOk = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                数据库空处理
------------------------------------------------------------------------------*/
void DatabaseEmpty(void)
{
    if((boSciRecOk) && (bSciRecCtrl[0] == 0x02) && (bSciRecCtrl[2] == 0x01) && (bSciRecCtrl[3] == 0x15))
    {
//        boDatabaseEmpty = 1;
        boSciRecOk = 0;
    }
}
