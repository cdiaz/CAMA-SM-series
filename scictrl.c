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
                ����ָ�1:N�ȶ�
------------------------------------------------------------------------------*/
void SendIdentifyPacket(void)                                                   //1:N�ȶ�
{
    StrcpyFromFlash(bSciTraBuf,coPacket2,24);
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                ����ָ�ָ�ƵǼ�
------------------------------------------------------------------------------*/
void SendEnrollPacket(BYTE bTemplateId_SendEnrollPacket)                        //ָ�ƵǼ�
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
                ����ָ�ɾ��ģ��
------------------------------------------------------------------------------*/
void SendClearTemplatePacket(BYTE bTemplateId_SendClearTemplatePacket)          //ɾ��ģ��
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
                ����ָ����ģ��
------------------------------------------------------------------------------*/
void SendClearAllTemplatePacket(void)                                           //���ģ��
{
    StrcpyFromFlash(bSciTraBuf,coPacket6,24);
    boSciTraStatus = 1;
    SciTraEnable();

}

/*------------------------------------------------------------------------------
                ����ָ���ȡָ��ģ��״̬
------------------------------------------------------------------------------*/
void SendGetTemplateStatusPacket(BYTE bTemplateId_SendGetTemplateStatusPacket)  //��ȡָ��ģ��״̬
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
                ����ָ��������ģ��
------------------------------------------------------------------------------*/
void SendGetBrokenTemplatePacket(void)                                          //�������ģ��״̬
{
    StrcpyFromFlash(bSciTraBuf,coPacket9,24);
    boSciTraStatus = 1;
    SciTraEnable();
}

/*------------------------------------------------------------------------------
                ����ָ����ð�ȫ�ȼ�
------------------------------------------------------------------------------*/
void SendSetSecurityLevelPacket(BYTE bSecurityLevel)                            //���ð�ȫ�ȼ�
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
                ����ָ����ó�ʱʱ��
------------------------------------------------------------------------------*/
void SendSetFingerTimeOutPacket(BYTE bFingerTimeOut)                            //���ó�ʱʱ��
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
                ����ָ�ȡ����ǰ����
------------------------------------------------------------------------------*/
void SendFPCancelPacket(void)                                                   //ȡ����ǰ����
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
                �������ݴ���
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
                ���ݿ�մ���
------------------------------------------------------------------------------*/
void DatabaseEmpty(void)
{
    if((boSciRecOk) && (bSciRecCtrl[0] == 0x02) && (bSciRecCtrl[2] == 0x01) && (bSciRecCtrl[3] == 0x15))
    {
//        boDatabaseEmpty = 1;
        boSciRecOk = 0;
    }
}
