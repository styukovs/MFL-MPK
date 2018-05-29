#ifndef COMMON_DRIVER_H
#define COMMON_DRIVER_H

#define SYSCLK              14318180                         //частота Fosc
#define FCY                 SYSCLK/2


#define BAUDRATE_19200      0
#define BAUDRATE_38400      1
#define BAUDRATE_57600      2
#define BAUDRATE_115200     3

#define BAUDRATE_19200_BRG  ((FCY/19200)/4) - 1
#define BAUDRATE_38400_BRG  ((FCY/38400)/4) - 1
#define BAUDRATE_57600_BRG  ((FCY/57600)/4) - 1
#define BAUDRATE_115200_BRG ((FCY/115200)/4) - 1




//******************************************************************************                                          
//*                          НОЖКИ МИКРОКОНТРОЛЛЕРА                            *
//******************************************************************************
#define HL1                 LATBbits.LATB6
#define HL2                 LATBbits.LATB7
#define M0                  LATAbits.LATA0
#define M1                  LATAbits.LATA1
#define DET                 PORTAbits.RA4
#define RX_FX604            M1 = 1; M0 = 0; __delay_ms(4);                   //настройка модема на прием
#define TX_FX604            M0 = 1; M1 = 0; __delay_us(200);                //настройка модема на передачу
#define MISO                PORTBbits.RB8
#define MOSI                LATBbits.LATB9
#define RDYN                PORTBbits.RB12
#define SCLK                LATBbits.LATB10
#define TX_RS485            LATBbits.LATB15
//пины, связанные с адресом модема
#define ADRES               PORTBbits.RB5
#define LOAD                LATBbits.LATB4
#define SS                  LATBbits.LATB11


//                                          КОНСТАНТЫ
// временные промежутки для FX604
#define Ts_us               1
#define Th_us               1
#define Td_us               1
#define Tchi_us             1


#define BUFF_SIZE           2048
#define SIZE_OF_HEAD        3
#define NULL_BYTE           0
#define DATA_SIZE_H         1
#define DATA_SIZE_L         2

// константы для таймера
#define TIMEOUT_FOR_19200     (16*FCY)/19200            // если время превышает время принятия 16 бит - то считаем, что пакет полностью принят
#define TIMEOUT_FOR_38400     (16*FCY)/38400
#define TIMEOUT_FOR_57600     (16*FCY)/57600
#define TIMEOUT_FOR_115200    (16*FCY)/115200



#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>

typedef struct
{
    uint8_t* ptr;
    uint16_t len;
    uint16_t cnt;
} data;

typedef struct
{
    uint8_t null_byte;
    uint16_t data_size;
    uint8_t CRC8;
    uint8_t data_buffer[BUFF_SIZE];
} frame;

#endif // COMMON_DRIVER_H