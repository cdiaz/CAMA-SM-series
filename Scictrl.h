#ifndef _scictrl_h_
#define _scictrl_h_

void SendIdentifyPacket(void);
void SendEnrollPacket(BYTE bTemplateId_SendEnrollPacket);
void SendClearTemplatePacket(BYTE bTemplateId_SendClearTemplatePacket);
void SendClearAllTemplatePacket(void);
void SendGetTemplateStatusPacket(BYTE bTemplateId_SendGetTemplateStatusPacket);
void SendGetBrokenTemplatePacket(void);
void SendSetSecurityLevelPacket(BYTE bSecurityLevel);
void SendSetFingerTimeOutPacket(BYTE bFingerTimeOut);
void SendSetBaudratePacket(void);
void SendFPCancelPacket(void);
void RecCtrl(void);

#endif
