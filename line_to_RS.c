#include <p24HJ128GP502.h>

#include "line_to_RS.h"
#include "CRC8.h"
#include "init.h"

static frame frame_to_RS;
static data data_to_RS;

static bool receive_frame(void);
static uint8_t receive_byte(void);
static void send_data(void);

void __attribute__((__interrupt__)) _CNInterrupt(void)
{

    if (0 == DET)
    {
        int_ON();
        return;
    }
    
    ClrWdt();
    HL1 = 1;
    if (receive_frame())
    {
        HL1 = 0;
        HL2 = 1;
        send_data();
    }
    HL1 = 0;
    HL2 = 0;

    IFS1bits.CNIF = 0;
}



static bool receive_frame(void)
{
    uint8_t head[SIZE_OF_HEAD];
    
    while (RDYN == 0)
    {
        receive_byte();
    }
    
    frame_to_RS.null_byte = receive_byte();
    frame_to_RS.data_size = receive_byte();
    frame_to_RS.data_size = (frame_to_RS.data_size << 8) | receive_byte();
    frame_to_RS.CRC8 = receive_byte();
    
    head[NULL_BYTE] = frame_to_RS.null_byte;
    head[DATA_SIZE_H] = frame_to_RS.data_size >> 8;
    head[DATA_SIZE_L] = frame_to_RS.data_size;
    
    if (frame_to_RS.CRC8 != Crc8(head, SIZE_OF_HEAD))
    {
        return 0;
    }
    
    data_to_RS.ptr = frame_to_RS.data_buffer;
    
    for (data_to_RS.cnt = 0; data_to_RS.cnt < frame_to_RS.data_size; (data_to_RS.cnt)++)
    {
        *(data_to_RS.ptr) = receive_byte();
        (data_to_RS.ptr)++;
    }
    data_to_RS.len = data_to_RS.cnt;
    
    return 1;
}

static void send_data(void)
{
    // RS-485
    U1MODEbits.STSEL = 1; // äâà ñòîïîâûõ áèòà
    U2MODEbits.STSEL = 1; // äâà ñòîïîâûõ áèòà
    TX_RS485 = 1;
    data_to_RS.ptr = frame_to_RS.data_buffer;
    data_to_RS.cnt = data_to_RS.len;
    while (data_to_RS.cnt)
    {
        U2TXREG = *(data_to_RS.ptr);
        U1TXREG = *(data_to_RS.ptr);
        (data_to_RS.ptr)++;
        (data_to_RS.cnt)--;
        while (U1STAbits.UTXBF || U2STAbits.UTXBF);
    }
    while (!U1STAbits.TRMT);
    TX_RS485 = 0;
    U1MODEbits.STSEL = 0; // îäèí ñòîïîâûé áèò
    U2MODEbits.STSEL = 0; // îäèí ñòîïîâûé áèò
}



static uint8_t receive_byte(void)
{
    uint8_t byte = 0, i;

    SCLK = 0;
    while(RDYN);                        ///// ÍÓÆÍÎ ×ÒÎÁÛ ÏÐÎÄÎËÆÅÍÈÅ ØËÎ ÎÒ ÇÀÄÍÅÃÎ ÔÐÎÍÒÀ (ÎÒ ÏÅÐÅÕÎÄÀ RDYN ÈÇ ÍÓËß Â ÅÄÈÍÈÖÓ)
    for (i = 0; i < 8; i++)
    {
        __delay_us(100);
        SCLK = 1;
        byte = byte | (MISO << i);
        __delay_us(100);
        SCLK = 0;
    }
    __delay_us(100);
    SCLK = 1;
    __delay_us(100);
    SCLK = 0;
    return byte;
}

