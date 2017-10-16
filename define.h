#ifndef _define_h_
#define _define_h_

typedef unsigned char       BYTE;		/* 1 byte */
typedef unsigned int        WORD;		/* 2 bytes */
typedef unsigned long       DWORD;	    /* 4 bytes */
typedef signed char         CHAR;		/* 1 byte */
typedef signed int          INT;	    /* 2 bytes */
typedef signed long         LONG;		/* 4 bytes */

#define LowByte(a)          ((BYTE)(a & 0x00ff))
#define HighByte(a)         ((BYTE)((a >> 8) & 0x00ff))

#define SYN1                0xAA
#define SYN2                0x55
#define DATAID              0x90
#define I2cWriteLable       0xa0
#define I2cReadLable        0xa1
#define I2cDataLen          0x20

#define ADC_VREF_TYPE       0xE7

#define EEPFIRSTADR         0x55
#define EEPFIRSTLAB         0xf5

#define Time1High           0xf8
#define Time1Low            0x30
#define ShenDiao            2
#define Time1High1          (((0x10000 - 4000000 / ShenDiao / 262) >> 8) & 0x00ff)
#define Time1Low1           ((0x10000 - 4000000 / ShenDiao / 262) & 0x00ff)
#define Time1High2          (((0x10000 - 4000000 / ShenDiao / 294) >> 8) & 0x00ff)
#define Time1Low2           ((0x10000 - 4000000 / ShenDiao / 294) & 0x00ff)
#define Time1High3          (((0x10000 - 4000000 / ShenDiao / 330) >> 8) & 0x00ff)
#define Time1Low3           ((0x10000 - 4000000 / ShenDiao / 330) & 0x00ff)
#define Time1High4          (((0x10000 - 4000000 / ShenDiao / 349) >> 8) & 0x00ff)
#define Time1Low4           ((0x10000 - 4000000 / ShenDiao / 349) & 0x00ff)
#define Time1High5          (((0x10000 - 4000000 / ShenDiao / 392) >> 8) & 0x00ff)
#define Time1Low5           ((0x10000 - 4000000 / ShenDiao / 392) & 0x00ff)
#define Time1High6          (((0x10000 - 4000000 / ShenDiao / 440) >> 8) & 0x00ff)
#define Time1Low6           ((0x10000 - 4000000 / ShenDiao / 440) & 0x00ff)
#define Time1High7          (((0x10000 - 4000000 / ShenDiao / 494) >> 8) & 0x00ff)
#define Time1Low7           ((0x10000 - 4000000 / ShenDiao / 493) & 0x00ff)
//#define Time1High Time1High4
//#define Time1Low Time1Low4

#define Code0               '0'
#define Code1               '1'
#define Code2               '2'
#define Code3               '3'
#define Code4               '*'
#define Code5               '#'

#define UPE                 2
#define OVR                 3
#define FE                  4
#define FRAMING_ERROR       (1 << FE)
#define PARITY_ERROR        (1 << UPE)
#define DATA_OVERRUN        (1 << OVR)

#define ERR_SUCCESS         0x00

#define BeepToBeepTime      000            //ms
#define I2cBufLen           32
#define Encrypt             0x7b

#define I2cInitOk           0xa5
#define FirstWrite          0x02
#define PasswordI2cAdr      0x08

#define ChkVal              PORTA_Bit4
#define Fin                 PORTA_Bit5              //Fin out
//#define Rin                 PORTA_Bit6              //Rin out

#define KeyC                PINB_Bit6
#define KeyW                PINB_Bit7

#define SCL                 PORTC_Bit0
#define SDAO                PORTC_Bit1
#define SDAI                PINC_Bit1
#define SDARW               DDRC_Bit1

#define BEEP                PORTD_Bit4
#define POWC                PORTD_Bit5
#define LEDBAK              PORTD_Bit6

#endif

