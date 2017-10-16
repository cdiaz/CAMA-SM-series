#ifndef _carlock_h_
#define _carlock_h_

#include "define.h"
extern bool boTest;

extern BYTE bSciTraBuf[];
extern BYTE bSciRecBuf[];
extern BYTE bSciRecCtrl[];
extern BYTE bKeyCode[];
extern BYTE bI2cDataBuf[];
extern BYTE bKeyCodeNum;
extern WORD wBeepTime;
extern WORD wNoBeepTime;
extern BYTE bPasswordNowIndex;
extern BYTE b5sSleep;
extern BYTE bErrorNum;
extern BYTE bShenDiaoHigh;
extern BYTE bShenDiaoLow;
extern BYTE bAdmin[];
extern BYTE bKeyData;
extern BYTE bKeyUpData;
extern BYTE bAdData;
extern BYTE bCalValTimes;

extern bool boSciTraStatus;
extern bool boSciRecStatus;
extern bool boSciRecOk;
extern bool boTime1ms;
extern bool boDelay1ms;
extern bool boKeyOk;
extern bool boKeyClearOk;
extern bool boKeyWakeupOk;
extern bool boChkAdminOk;

extern bool boBeep;
//extern bool boBeepOut;

extern bool boBeepKey;
extern bool boBeepError;
extern bool boBeepSuccess;
extern bool boBeepLowpow;
extern bool boDatabaseEmpty;
extern bool boSendEnrollPacket;
extern bool boSendIdentifyPacket;
extern bool boSendFPCancelPacket;
extern bool boAddAppoint;
extern bool boOpenDoor;

extern bool boSleepSys;
extern bool boOutSleep;
extern bool boAllaywayMode;
extern bool boIntoSleep;
/*------------------------------------------------------------------------------
                内部E2变量定义
------------------------------------------------------------------------------*/
extern __eeprom BYTE bFirstInitE2;
extern __eeprom BYTE bAdminE2[8];
extern __eeprom bool boAllaywayModeE2;


/*------------------------------------------------------------------------------
                常量定义
------------------------------------------------------------------------------*/

extern BYTE __flash coPacket2[];
extern BYTE __flash coPacket3[];
extern BYTE __flash coPacket5[];
extern BYTE __flash coPacket6[];
extern BYTE __flash coPacket8[];
extern BYTE __flash coPacket9[];
extern BYTE __flash coPacket12[];
extern BYTE __flash coPacket14[];
extern BYTE __flash coPacket48[];

#endif