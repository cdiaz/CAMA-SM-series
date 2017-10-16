/*******************************************************************************
This program was produced by the
CodeWizardAVR V1.24.8d Professional
Automatic Program Generator
?Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2007-9-6
Author  : Xucf                            
Company : Xucf                            
Comments: 


Chip type           : ATmega16L
Program type        : Application
Clock frequency     : 10.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*******************************************************************************/

#include <iom16.h>
#include <stdbool.h>
#include <ina90.h>
#include <intrinsics.h>
#include "define.h"
#include "Var.h"
#include "Suo.h"
#include "main.h"
#include "I2c.h"
#include "Setup.h"
#include "Scictrl.h"
#include "Beepkey.h"

/*------------------------------------------------------------------------------
                主函数
------------------------------------------------------------------------------*/
void main(void)
{
    static bool boSleepTime = 0;
    
    // Declare your local variables here

    // Input/Output Ports initialization
    // Port A initialization
    // Func7 = In Func6 = Out Func5 = Out Func4 = Out Func3 = In Func2 = In Func1 = In Func0 = In 
    // State7 = T State6 = 0  State5 = 0  State4 = 1 State3 = T State2 = T State1 = T State0 = T 
    PORTA = 0x10;
    DDRA = 0x70;

    // Port B initialization
    // Func7 = In Func6 = In Func5 = In Func4 = In Func3 = In Func2 = In Func1 = In Func0 = In 
    // State7 = P State6 = P State5 = P State4 = P State3 = P State2 = P State1 = P State0 = P 
    PORTB = 0xff;
    DDRB = 0x00;

    // Port C initialization
    // Func7 = In Func6 = In Func5 = In Func4 = In Func3 = In Func2 = In Func1 = Out Func0 = Out
    // State7 = 0 State6 = 0 State5 = T  State4 = T State3 = T State2 = T State1 = 1  State0 = 1 
    PORTC = 0x03;
    DDRC = 0x03;

    // Port D initialization
    // Func7 = In Func6 = Out Func5 = Out Func4 = Out Func3 = Out Func2 = In Func1 = Out Func0 = In 
    // State7 = P State6 = 1 State5 = 1  State4 = 0  State3 = 0 State2 = P  State1 = 1  State0 = T 
    PORTD = 0xe6;
    DDRD = 0x79;

    SFIOR &= 0xfb;
    
    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // INT2: Off
    MCUCR = 0x00;
    MCUCSR = 0x00;
    
    _CLI();
    
    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK = 0x00;
    
    SciInit();
//    AnaCompInit();
    Time0OvfInit();
    Time1OvfInit();
    
    // Global enable interrupts
    _SEI();
    Delayms(600);
    AdcInit();
    bKeyData = PINB & 0x3f;
    bKeyUpData = PINB & 0x3f;
    
    while(bCalValTimes < 10);
    if((bI2cDataBuf[0] + bI2cDataBuf[1] + bI2cDataBuf[2] + bI2cDataBuf[3] + bI2cDataBuf[4] + 
       bI2cDataBuf[5] + bI2cDataBuf[6] + bI2cDataBuf[7] + bI2cDataBuf[8] + bI2cDataBuf[9]) / 10 < 0xcd)
    {
        boBeepLowpow = 1;
    }
    else
    {
        boBeepKey = 1;
    }
    ChkVal = 0;
    
    Delayms(100);
    
    E2promInit();
    
    if(boAllaywayModeE2 || boDatabaseEmpty)
    {
        Fin = 1;
        boOpenDoor = 1;
    }
    else
    {
        boSendIdentifyPacket = 1;
    }
    
    while(1)
    {
        ChkKey();
        GetKey();
        BeepKey();
        RecCtrl();
        DatabaseEmpty();
        
        BeepError();
        BeepSuccess();
        BeepLowpow();
        
        SendFPCancelPacket();
        ChkAdmin();
        FactorySet();
        AllaywayMode();
        ChangePassword();
        Add();
        Del();
        OpenDoor();
        
        if(boSleepSys && (!boBeepSuccess) && (!boBeepError) && (!boBeep) && (bKeyUpData == 0x3f) && (!boKeyClearOk))
        {
            boIntoSleep = 1;
            boAllaywayMode = boAllaywayModeE2;
            boSleepTime = 0;
            IntoSleep();
            while(!boOutSleep)
            {
                if(boAllaywayMode)
                {
                    if(boSleepTime)
                    {
                        DDRD |= 0x30;
                        POWC = 1;
                        boBeepKey = 1;
                        while(boBeepKey || boBeep)
                        {
                            BeepKey();
                        }
                        POWC = 0;
                        DDRD &= 0xcf;
                    }
                    boSleepTime ^= 1;
                    __sleep();
                    Delaynop();
                    continue;
                }
            }
            WDTCR = 0x18;
            WDTCR = 0x08;
            while(1);
        }
    }
}

/*------------------------------------------------------------------------------
                串口函数（包括串口接收、串口发送和初始化）
------------------------------------------------------------------------------*/
// USART Receiver interrupt service routine
#pragma vector = USART_RXC_vect
__interrupt void SciRx(void)
{
    BYTE bSciRecData;
    BYTE bUCStatus;
    static BYTE bSciRec = 0;
    
    bSciRecData = UDR;
    bUCStatus = UCSRA;
    if((bUCStatus & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) != 0)
    {
        UCSRA &= 0xe3;
    }
    
    if(!bSciRec)
    {
        if(bSciRecData == SYN1)
        {
            bSciRecBuf[bSciRec ++] = bSciRecData;
        }
        else
        {
            bSciRec = 0;
        }
    }
    else if(bSciRec == 1)
    {
        if(bSciRecData == SYN2)
        {
            bSciRecBuf[bSciRec ++] = bSciRecData;
        }
        else
        {
            bSciRec = 0;
        }
    }
    else
    {
        bSciRecBuf[bSciRec ++] = bSciRecData;
        if(bSciRec >= 24)
        {
            boSciRecStatus = 1;
            bSciRec = 0;
        }
    }
}

/*------------------------------------------------------------------------------
                发送中断处理
------------------------------------------------------------------------------*/
// USART Transmitter interrupt service routine
#pragma vector = USART_TXC_vect
__interrupt void SciTx(void)
{
    static BYTE bSciTra = 1;
    
    if(boSciTraStatus && (bSciTra < 24))
    {
        UDR = bSciTraBuf[bSciTra ++];
    }
    else
    {
        boSciTraStatus = 0;
        bSciTra = 1;
    }
}

/*------------------------------------------------------------------------------
                串口初始化
------------------------------------------------------------------------------*/
void SciInit(void)
{
    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: On
    // USART Mode: Asynchronous
    // USART Baud rate: 115200
    
    UCSRA = 0x42;         //UCSRA.2(U2X) = 1
    UCSRB = 0xD8;
    UCSRC = 0x86;
    UBRRH = 0x00;
    UBRRL = 0x33;         //fosc=8M baud=115200(U2X = 1)
}

/*------------------------------------------------------------------------------
                发送使能
------------------------------------------------------------------------------*/
void SciTraEnable(void)
{
	if(boSciTraStatus)
	{
		UDR = bSciTraBuf[0];
	}
}
/*------------------------------------------------------------------------------
                AD转换器
------------------------------------------------------------------------------*/
// ADC interrupt service routine
#pragma vector = ADC_vect
__interrupt void Adcon(void)
{
	bAdData = ADCH;
    
    if(bCalValTimes < 10)
    {
        bI2cDataBuf[bCalValTimes] = bAdData;
        bCalValTimes ++;
    	ADMUX = ADC_VREF_TYPE;
        ADCSRA |= 0x40;
    }
    else
    {
        ADMUX = 0x00;
        ADCSRA = 0x00;
    }
    
}

/*------------------------------------------------------------------------------
                ad转换初始化
------------------------------------------------------------------------------*/
void AdcInit(void)
{
	// ADC initialization
	// ADC Clock frequency: 156.250 kHz
	// ADC Voltage Reference: Int., cap. on AREF
	// ADC Auto Trigger Source: None
	// Only the 8 most significant bits of
	// the AD conversion result are used
	SFIOR &= 0x0f;
	ADMUX = ADC_VREF_TYPE;
	ADCSRA = 0xce;
    ADCSRA |= 0x08;
}

/*------------------------------------------------------------------------------
                EEPROM读写函数
------------------------------------------------------------------------------*/
/*void EEpWrite(BYTE bEEpWriteL,BYTE bEEpWriteD)
{
    while(EECR & 0x02);
    EEAR = bEEpWriteL;
    EEDR = bEEpWriteD;
    EECR |= 0x04;
    EECR |= 0x02;
}

BYTE EEpRead(BYTE bEEpReadL)
{
    while(EECR & 0x02);
    EEAR = bEEpReadL;
    EECR |= 0x01;
    return(EEDR);
}*/

/*------------------------------------------------------------------------------
                定时计数器0溢出中断（包括中断和初始化）
------------------------------------------------------------------------------*/
#pragma vector = TIMER0_OVF_vect
__interrupt void Time0Ovf(void)           //1ms time0 interrupt
{
    static BYTE bDelay100ms = 0;
    static BYTE bLedDelay200ms = 0;
    static BYTE bOpenDoorDelay3s = 0;
    
    TCNT0 = 0x83;               //set counter 1ms
    
    boTime1ms = 1;
    boDelay1ms = 1;
    
    if(boBeep)
    {
        if(wNoBeepTime)
        {
            Time1Disable();
            wNoBeepTime --;
        }
        else
        {
            if(wBeepTime)
            {
                Time1Enable();
                wBeepTime --;
            }
            else
            {
                Time1Disable();
                boBeep = 0;
            }
        }
    }
    
    if(boIntoSleep)
    {
        return;
    }
    
    bDelay100ms ++;
    if(bDelay100ms >= 100)
    {
        bDelay100ms = 0;
        
        if(!bKeyCodeNum)
        {
            if(boDatabaseEmpty)
            {
                bLedDelay200ms ++;
                if(bLedDelay200ms >= 2)
                {
                    LEDBAK ^= 1;
                    bLedDelay200ms = 0;
                }
            }
        }
        else
        {
            LEDBAK = 1;
        }
        
        if(boOpenDoor)
        {
            bOpenDoorDelay3s ++;
            if(bOpenDoorDelay3s >= 30)
            {
                bOpenDoorDelay3s = 0;
                boOpenDoor = 0;
                Fin = 0;
                if((!boAllaywayModeE2) && (!boDatabaseEmpty))
                {
                    boSleepSys = 1;
                }
            }
        }
        
        b5sSleep ++;
        if(b5sSleep >= 50)
        {
            b5sSleep = 0;
            boSleepSys = 1;
        }
    }
}

/*------------------------------------------------------------------------------
                定时器0初始化
------------------------------------------------------------------------------*/
void Time0OvfInit(void)
{
    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: Timer 0 Stopped
    // Mode: Normal top = FFh
    // OC0 output: Disconnected
    
    TCCR0 = 0x00;               //stop
    TCNT0 = 0x83;               //set counter 1ms
//    OCR0 = 0x00;                //set compare
    TCCR0 = 0x03;               //start timer  fosc / 256
    TIMSK |= 0x01;              //time0 overflow interrupt enable
}

/*------------------------------------------------------------------------------
                INT中断
------------------------------------------------------------------------------*/
// External Interrupt 0 service routine
#pragma vector = INT0_vect
__interrupt void Int0(void)
{
    boOutSleep = 1;
}

// External Interrupt 1 service routine
#pragma vector = INT1_vect
__interrupt void Int1(void)
{
    boOutSleep = 1;
}

/*------------------------------------------------------------------------------
                外部中断初始化
------------------------------------------------------------------------------*/
void IntInit(void)
{
    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Falling Edge
    // INT1: On
    // INT1 Mode: Falling Edge
    // INT2: Off
    GICR |= 0xC0;
    MCUCR = 0x0A;
    MCUCSR = 0x00;
    GIFR = 0xC0;
}

/*------------------------------------------------------------------------------
                定时计数器1溢出中断（包括中断和初始化）
------------------------------------------------------------------------------*/
#pragma vector = TIMER1_OVF_vect
__interrupt void Time1Ovf(void)
{
    BEEP ^= 1;
    TCNT1H = bShenDiaoHigh;
    TCNT1L = bShenDiaoLow;
    TIFR |= 0x04;
}

void Time1OvfInit(void)
{
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer 1 Stopped
    // Mode: Normal top=FFFFh
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer 1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
  
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1H = 0xff;
    TCNT1L = 0xff;
    ICR1H = 0x00;
    ICR1L = 0x00;
    OCR1AH = 0x00;
    OCR1AL = 0x00;
    OCR1BH = 0x00;
    OCR1BL = 0x00;
}

void Time1Enable(void)
{
    TCCR1B = 0x01;
    TIMSK |= 0x04;
}

void Time1Disable(void)
{
    TCCR1B = 0x00;
    TIMSK &= 0xfb;
    BEEP = 0;
}

/*------------------------------------------------------------------------------
                定时计数器2溢出中断（包括中断和初始化）
------------------------------------------------------------------------------*/
#pragma vector = TIMER2_OVF_vect
__interrupt void Time2Ovf(void)
{
//    TCNT2 = 0x00;
}

void Time2OvfInit(void)
{
    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: 7.813 kHz
    // Mode: Normal top=FFh
    // OC2 output: Disconnected
    
    ASSR = 0x08;
    SFIOR |= 0x02;
    TCCR2 = 0x07;
    TCNT2 = 0x00;
    OCR2 = 0x00;
    while(ASSR & 0x07);
    TIFR &= 0x3f;
    TIMSK |= 0x40;
}

/*------------------------------------------------------------------------------
                初始化外部存储区
------------------------------------------------------------------------------*/
void E2promInit(void)
{
    BYTE bTemp_E2promInit;
        
    I2cReadPage(bI2cDataBuf,1,0,FirstWrite);
    if((bI2cDataBuf[0] == I2cInitOk) && (bFirstInitE2 == I2cInitOk))
    {
        bAdmin[0] = bAdminE2[0];
        bAdmin[1] = bAdminE2[1];
        bAdmin[2] = bAdminE2[2];
        bAdmin[3] = bAdminE2[3];
        bAdmin[4] = bAdminE2[4];
        bAdmin[5] = bAdminE2[5];
        bAdmin[6] = bAdminE2[6];
        bAdmin[7] = bAdminE2[7];
        
        bPasswordNowIndex = GetEmptyRecord();
        
        while((!boSciRecOk) || (bSciRecCtrl[0] != 0x09) || (bSciRecCtrl[2] != ERR_SUCCESS))
        {
            SendGetBrokenTemplatePacket();
            while(!boSciRecOk)
            {
                RecCtrl();
            }
        }
        boSciRecOk = 0;
        
        for(bTemp_E2promInit = 0;bTemp_E2promInit < 255;bTemp_E2promInit ++)
        {
            I2cReadPage(bI2cDataBuf,1,HighByte((WORD)(bTemp_E2promInit) * 8 + PasswordI2cAdr),
                         LowByte((WORD)(bTemp_E2promInit) * 8 + PasswordI2cAdr));
            if(bI2cDataBuf[0] != 0xff)
            {
                break;
            }
        }
        if(bTemp_E2promInit >= 255)
        {
            boDatabaseEmpty = 1;
        }
    }
    else
    {
        StrCls(bAdmin,8,'1');
        StrcpyToE2prom(bAdminE2,bAdmin,8);
        
        for(bTemp_E2promInit = 0;bTemp_E2promInit < I2cBufLen / 2;bTemp_E2promInit++)
        {
            bI2cDataBuf[bTemp_E2promInit] = 0xff;
        }
        for(bTemp_E2promInit = 0;bTemp_E2promInit < 2048 / (I2cBufLen / 2);bTemp_E2promInit ++)
        {
            I2cWritePage(bI2cDataBuf,I2cBufLen / 2,HighByte((WORD)(bTemp_E2promInit) * I2cBufLen / 2),
                         LowByte((WORD)(bTemp_E2promInit) * I2cBufLen / 2));
        }
        
        bPasswordNowIndex = 1;
        boAllaywayModeE2 = 0;
        
        while(1)
        {
            SendClearAllTemplatePacket();
            while(!boSciRecOk)
            {
                RecCtrl();
            }
            if(boSciRecOk && (bSciRecCtrl[0] == 0x06))
            {
                if(!bSciRecCtrl[2])
                {
                    break;
                }
                else
                {
                    boSciRecOk = 0;
                    continue;
                }
            }
        }
        
        boDatabaseEmpty = 1;
        bFirstInitE2 = I2cInitOk;
        bI2cDataBuf[0] = I2cInitOk;
        I2cWritePage(bI2cDataBuf,1,0,FirstWrite);
    }
}

void IntoSleep(void)
{
    // Declare your local variables here
    
    // Input/Output Ports initialization
    // Port A initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTA = 0x00;
    DDRA = 0x00;
    
    // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=0 State0=0 
    PORTC = 0x00;
    DDRC = 0x00;
    
    // Port D initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=P State2=P State1=T State0=T 
    PORTD = 0x04;
    DDRD = 0x08;
    
    if(!boAllaywayMode)
    {
        // Timer/Counter 0 initialization
        // Clock source: System Clock
        // Clock value: Timer 0 Stopped
        // Mode: Normal top=FFh
        // OC0 output: Disconnected
        TCCR0 = 0x00;
        TCNT0 = 0x00;
        OCR0 = 0x00;
    
        // Timer/Counter 1 initialization
        // Clock source: System Clock
        // Clock value: Timer 1 Stopped
        // Mode: Normal top=FFFFh
        // OC1A output: Discon.
        // OC1B output: Discon.
        // Noise Canceler: Off
        // Input Capture on Falling Edge
        // Timer 1 Overflow Interrupt: Off
        // Input Capture Interrupt: Off
        // Compare A Match Interrupt: Off
        // Compare B Match Interrupt: Off
        TCCR1A = 0x00;
        TCCR1B = 0x00;
        TCNT1H = 0x00;
        TCNT1L = 0x00;
        ICR1H = 0x00;
        ICR1L = 0x00;
        OCR1AH = 0x00;
        OCR1AL = 0x00;
        OCR1BH = 0x00;
        OCR1BL = 0x00;
    }
    
    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: Timer 2 Stopped
    // Mode: Normal top=FFh
    // OC2 output: Disconnected
    ASSR = 0x00;
    TCCR2 = 0x00;
    TCNT2 = 0x00;
    OCR2 = 0x00;
    
    while((!KeyW) && ((PINB & 0x3f) == 0x3f));
    // Port B initialization
    // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
    // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
    PORTB = 0x00;
    DDRB = 0xFF;
    
    PORTD |= 0x08;
    DDRD &= 0xF7;    

    // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR = 0x80;
    SFIOR = 0x00;
    
    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: Off
    // USART Transmitter: Off
    // USART Mode: Asynchronous
    UCSRA = 0x00;
    UCSRB = 0x00;
    UCSRC = 0x00;
    
    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Falling Edge
    // INT1: On
    // INT1 Mode: Falling Edge
    // INT2: Off
    MCUCR = 0x00;
    MCUCSR = 0x00;
    GIFR = 0xC0;
    GICR |= 0xC0;

    //adc initialization
	SFIOR &= 0x0f;
	ADMUX = 0;
	ADCSRA = 0;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    if(!boAllaywayMode)
    {
        TIMSK = 0x00;
    }
    else
    {
        TIMSK &= 0x3f;
    }
    MCUCSR |= 0x80;
    
    boOutSleep = 0;
    if(!boAllaywayMode)
    {
        MCUCR |= 0x60;
    }
    else
    {
        Time2OvfInit();
        MCUCR |= 0x70;
    }
    __sleep();
}

/*------------------------------------------------------------------------------
                延时程序（ms)
------------------------------------------------------------------------------*/
void Delayms(WORD wDelayTime)
{
    do
    {
        boDelay1ms = 0;
        while(!boDelay1ms);
    }
    while(wDelayTime --);
}

/*------------------------------------------------------------------------------
                串拷贝(从flash memory）
------------------------------------------------------------------------------*/
void StrcpyFromFlash(BYTE* pData1_StrcpyFromFlash,BYTE __flash* pData2_StrcpyFromFlash,BYTE bLen_StrcpyFromFlash)
{
    BYTE bTemp_StrcpyFromFlash;
    
    for(bTemp_StrcpyFromFlash = 0;bTemp_StrcpyFromFlash < bLen_StrcpyFromFlash;bTemp_StrcpyFromFlash ++)
    {
        pData1_StrcpyFromFlash[bTemp_StrcpyFromFlash] = pData2_StrcpyFromFlash[bTemp_StrcpyFromFlash];
    }
}

/*------------------------------------------------------------------------------
                串拷贝(到eeprom memory）
------------------------------------------------------------------------------*/
void StrcpyToE2prom(BYTE __eeprom* pData1_StrcpyToE2prom,BYTE* pData2_StrcpyToE2prom,BYTE bLen_StrcpyToE2prom)
{
    BYTE bTemp_StrcpyToE2prom;
    
    for(bTemp_StrcpyToE2prom = 0;bTemp_StrcpyToE2prom < bLen_StrcpyToE2prom;bTemp_StrcpyToE2prom ++)
    {
        pData1_StrcpyToE2prom[bTemp_StrcpyToE2prom] = pData2_StrcpyToE2prom[bTemp_StrcpyToE2prom];
    }
}

/*------------------------------------------------------------------------------
                串比较
------------------------------------------------------------------------------*/
bool StrComp(BYTE* pData1_StrComp,BYTE* pData2_StrComp,BYTE bLen_StrComp)
{
    BYTE bTemp_StrComp;
    
    for(bTemp_StrComp = 0;bTemp_StrComp < bLen_StrComp;bTemp_StrComp ++)
    {
        if(pData1_StrComp[bTemp_StrComp] != pData2_StrComp[bTemp_StrComp])
        {
            return(0);
        }
    }
    return(1);
}

/*------------------------------------------------------------------------------
                串填补（把某个数组填上一样的字符）
------------------------------------------------------------------------------*/
void StrCls(BYTE* pData_StrCls,BYTE bLen_StrCls,BYTE bData_StrCls)
{
    BYTE bTemp_StrCls;
    
    for(bTemp_StrCls = 0;bTemp_StrCls < bLen_StrCls;bTemp_StrCls ++)
    {
        pData_StrCls[bTemp_StrCls] = bData_StrCls;
    }
}

/*------------------------------------------------------------------------------
                串范围校验（确认字符串中的字符处在某范围内）
------------------------------------------------------------------------------*/
bool StrChk(BYTE* pData_StrChk,BYTE bLen_StrChk)
{
    BYTE bTemp_StrChk;
    
    for(bTemp_StrChk = 0;bTemp_StrChk < bLen_StrChk;bTemp_StrChk ++)
    {
        if((pData_StrChk[bTemp_StrChk] < Code0) || (pData_StrChk[bTemp_StrChk] > Code3))
        {
            return(0);
        }
    }
    return(1);
}
/*------------------------------------------------------------------------------
                找寻当前数据库中的第一个空记录
------------------------------------------------------------------------------*/
BYTE GetEmptyRecord(void)
{
    WORD wTemp_GetEmptyRecord;
    
    for(wTemp_GetEmptyRecord = PasswordI2cAdr;wTemp_GetEmptyRecord < 2048;wTemp_GetEmptyRecord += 8)
    {
        I2cReadPage(bI2cDataBuf,1,HighByte(wTemp_GetEmptyRecord),LowByte(wTemp_GetEmptyRecord));
        if(bI2cDataBuf[0] == 0xff)
        {
            return((BYTE)((wTemp_GetEmptyRecord - PasswordI2cAdr) / 8) + 1);
        }
    }
    return(0);
}
