#ifndef _main_h_
#define _main_h_

void SciInit(void);
void AnaCompInit(void);
void Time0OvfInit(void);
void Time1OvfInit(void);
void Time2OvfInit(void);
void Time1Enable(void);
void Time1Disable(void);
void SciCtrl(void);
void AdcInit(void);
void Sci1TraEnable(void);
void StrcpyFromFlash(BYTE* pData1_StrcpyFromFlash,BYTE __flash* pData2_StrcpyFromFlash,BYTE  bLen_StrcpyFromFlash);
void StrcpyToE2prom(BYTE __eeprom* pData1_StrcpyToE2prom,BYTE* pData2_StrcpyToE2prom,BYTE bLen_StrcpyToE2prom);
bool StrComp(BYTE* pData1_StrComp,BYTE* pData2_StrComp,BYTE bLen_StrComp);
void StrCls(BYTE* pData_StrCls,BYTE bLen_StrCls,BYTE bData_StrCls);
bool StrChk(BYTE* pData_StrChk,BYTE bLen_StrChk);
void SciTraEnable(void);
void Delayms(WORD wDelayTime);
void E2promInit(void);
void IntoSleep(void);
void DatabaseEmpty(void);
BYTE GetEmptyRecord(void);

#endif
