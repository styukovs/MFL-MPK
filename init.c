#include "init.h"

static void init_osc(void); //������������� ���������� �������
static void remap(void); //��������
static void init_address_baudrate(void); //������������� ������/�������� ������
static void init_ports(void); //������������� ������ (��������� �� ����-�����)
static void init_timer(void); //������������� �������1 (��������� �������), �������� �� ����� ����� ��������� ��������, ��� ������������ ������� �������, ��� ���� � RS ������ ���������
static void init_UART1(void);  // RS-485
static void init_UART2(void);  // RS-232

static uint8_t get_baudrate(void);

static struct ADDRBAUDRATEBITS
{

    union
    {
        uint8_t addr_baudrate;

        struct
        {
            unsigned addr : 6;
            unsigned baudrate : 2;
        };
    };
} ADDRBAUDRATEbits;

void init_modem()
{
    init_osc(); //������������� ���������� �������
    remap(); //��������
    init_ports(); //������������� ������ (��������� �� ����-�����)
    init_address_baudrate(); //������������� ������/�������� ������
    init_UART1();
    init_UART2();
    init_timer(); //������������� �������1 (��������� �������)
    
    RX_FX604;
    TX_RS485 = 0;
}

/* ��������� ���������� (������� Fcy)       �� ������ �������� � "common.h" SYSCLK
Fosc = Fin * M/(N1*N2), Fcy = Fosc/2,                       
Fosc = 3579545 * 64/(2*8) = 14318180 Hz
Fcy = 14318180/2 = 7159090 Hz */
static void init_osc(void)
{
    PLLFBD = 62; // M = 64,   M = PLLFBD + 2
    CLKDIVbits.PLLPRE = 0; // N1 = 2
    CLKDIVbits.PLLPOST = 3; // N2 = 8
    OSCTUN = 0; // ��������� FRC ����������
    RCONbits.SWDTEN = 0; // ���������� WDT
    while (0 == OSCCONbits.LOCK); // ����, ���� ����������� �������
    return;
}

// ����� �����

static void remap(void)
{
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6)); // ���������� ������

    RPINR18bits.U1RXR = 2; // U1RX �� 6 ���� (RP2)
    RPOR1bits.RP3R = 3; // U1TX �� 7 ���� (RP3)

    RPINR19bits.U2RXR = 13; // U2RX �� 24 ���� (RP13)
    RPOR7bits.RP14R = 5; // U2TX �� 25 ���� (RP14)

    __builtin_write_OSCCONL(OSCCON | (1 << 6)); // ���������� ������
    return;
}

// ������������� ������

static void init_address_baudrate(void)
{
    int8_t clk = 7;

    SCLK = 0;
    LOAD = 1;
    LOAD = 0;
    LOAD = 1;

    ADDRBAUDRATEbits.addr_baudrate = 0;
    ADDRBAUDRATEbits.addr_baudrate = ADDRBAUDRATEbits.addr_baudrate | (ADRES << clk);

    for (clk = 6; clk >= 0; --clk)
    {
        SCLK = 1;
        SCLK = 0;
        ADDRBAUDRATEbits.addr_baudrate = ADDRBAUDRATEbits.addr_baudrate | (ADRES << clk);
    }
    ADDRBAUDRATEbits.addr_baudrate ^= 0xFF; //����������� ����, �.�. ���������� ������������� ����������� � ���� 
}

// RS-485

void init_UART1(void)
{
    switch (get_baudrate())
    {
    case BAUDRATE_19200:
        U1BRG = BAUDRATE_19200_BRG;
        break;
    case BAUDRATE_38400:
        U1BRG = BAUDRATE_38400_BRG;
        break;
    case BAUDRATE_57600:
        U1BRG = BAUDRATE_57600_BRG;
        break;
    case BAUDRATE_115200:
        U1BRG = BAUDRATE_115200_BRG;
        break;
    default:
        break;
    }

    U1MODE = 0;
    U1MODEbits.BRGH = 1; // ���������������� ����� UART
    U1MODEbits.STSEL = 0; // ���� �������� ����

    U1STA = 0;
    U1STAbits.URXISEL = 0; // ���������� �� ��������� ����� ������� ��������� �����
    
    U1MODEbits.UARTEN = 1; // ��������� ������ UART
    U1STAbits.UTXEN = 1; // ���������� �� �������� UART
}

//RS-232

void init_UART2(void)
{
    switch (get_baudrate())
    {
    case BAUDRATE_19200:
        U2BRG = BAUDRATE_19200_BRG;
        break;
    case BAUDRATE_38400:
        U2BRG = BAUDRATE_38400_BRG;
        break;
    case BAUDRATE_57600:
        U2BRG = BAUDRATE_57600_BRG;
        break;
    case BAUDRATE_115200:
        U2BRG = BAUDRATE_115200_BRG;
        break;
    default:
        break;
    }

    U2MODE = 0;
    U2MODEbits.BRGH = 1; // ���������������� ����� UART
    U2MODEbits.STSEL = 0; // ���� �������� ����
    
    U2STA = 0;
    U2STAbits.URXISEL = 0; // ���������� �� ��������� ����� ������� ��������� �����
        
    U2MODEbits.UARTEN = 1; // ��������� ������ UART
    U2STAbits.UTXEN = 1; // ���������� �� �������� UART
}

static void init_ports(void)
{
    AD1PCFGL = 0xFFFF; //��� ����� �� �������� �����
    M1 = 1; 
    M0 = 0;
    
    LATBbits.LATB14 = 1; //RB14 (U2TX), �������� � RS-232
    LATBbits.LATB3 = 1; //RB3 (U1TX), �������� � RS-485, ����� ��������� "1", �.�. ����� ���������� �� ����� � ���������� UARTa �� ������ ���� ����
    TX_RS485 = 0;
    
    TRISAbits.TRISA0 = 0; //RA0 (M0) �� �����, M0 - �����/�������� FX604
    TRISAbits.TRISA1 = 0; //RA1 (M1) �� �����, M1 - �����/�������� FX604
    TRISAbits.TRISA4 = 1; //RA4 (DET) �� ����, �������� ������� � �����
    TRISBbits.TRISB2 = 1; //RB2 (U1RX) �� ����, UART1 - ����� �� RS-485
    TRISBbits.TRISB3 = 0; //RB3 (U1TX) �� �����, UART1 - �������� � RS-485
    TRISBbits.TRISB4 = 0; //RB4 (LOAD) �� �����, LOAD - ����� ��� ���������� ��������, ������� ���������� ��������� �������������� SA2
    TRISBbits.TRISB5 = 1; //RB5 (ADRES) �� ����, ADRES - � ������ ���������� ��������, ������� ���������� ��������� �������������� SA2
    TRISBbits.TRISB6 = 0; //RB6 (HL1) �� �����, HL1 - ���������
    TRISBbits.TRISB7 = 0; //RB7 (HL2) �� �����, HL2 - ���������
    TRISBbits.TRISB8 = 1; //RB8 (MISO) �� ����, MISO ������ � �����
    TRISBbits.TRISB9 = 0; //RB9 (MOSI) �� �����, MOSI ������ � �����
    TRISBbits.TRISB10 = 0; //RB10 (SCLK) �� �����, SCLK - ������� � ������ FX604 � � ���������� ��������
    TRISBbits.TRISB11 = 0; //RB11 (SS) �� �����, SS - ���������� � RXEQ ������ FX604, ��� ��������� ������� ��� ���������
    TRISBbits.TRISB12 = 1; //RB12 (RDYN) �� ����, RDYN - Ready for Data Transfer, ���������� �������� ������ � �����
    TRISBbits.TRISB13 = 1; //RB13 (U2RX) �� ����, UART2 - ����� �� RS-232
    TRISBbits.TRISB14 = 0; //RB14 (U2RX) �� ����, UART2 - �������� � RS-232
    TRISBbits.TRISB15 = 0; //RB15 (T/R) �� �����, T/R - ��������� �� ��������/����� �������� RS-485 (1 - �������� � RS-485; 0 - �����)

    SS = 1; //��������� ������� ��� ��������� FX604
}


static void init_timer(void)
{
    
    switch (get_baudrate())
    {
    case BAUDRATE_19200:
        PR1 = TIMEOUT_FOR_19200;
        break;
    case BAUDRATE_38400:
        PR1 = TIMEOUT_FOR_38400;
        break;
    case BAUDRATE_57600:
        PR1 = TIMEOUT_FOR_57600;
        break;
    case BAUDRATE_115200:
        PR1 = TIMEOUT_FOR_115200;
        break;
    default:
        break;
    }
}


void int_ON(void)
{
    IPC16bits.U1EIP = 6; //��������� ���������� �� ������ UART1 (RS-485)
    IPC16bits.U2EIP = 6; //��������� ���������� �� ������ UART2 (RS-232)
    IPC2bits.U1RXIP = 5;
    IPC7bits.U2RXIP = 5;
    
    IFS4bits.U1EIF = 0; // ����� ����� ������ � ��������� UART1 (RS-485)
    IFS4bits.U2EIF = 0; // ����� ����� ������ � ��������� UART2 (RS-232)
    IFS1bits.CNIF = 0; //����� ����� � ���������� �� ��������� ������ (����������)
    IFS0bits.U1RXIF = 0; // ����� ����� � ��������� UART1 (RS-485)
    IFS1bits.U2RXIF = 0; // ����� ����� � ��������� UART2 (RS-232)
    
    CNEN1bits.CN0IE = 1; //���������� ���������� �� ��������� ������ DET
    IEC1bits.CNIE = 1; //���������� ���������� �� ��������� ������ (����������)

    IEC0bits.U1RXIE = 1; // ���������� ���������� �� ��������� UART1 (RS-485)
    IEC1bits.U2RXIE = 1; // ���������� ���������� �� ��������� UART2 (RS-232)
    
    IEC4bits.U1EIE = 1; //���������� ���������� �� ������ UART1 (RS-485)
    IEC4bits.U2EIE = 1; //���������� ���������� �� ������ UART2 (RS-232)
}

void int_OFF(void)
{
    IEC0bits.U1RXIE = 0; // ���������� ���������� �� ��������� UART1 (RS-485)
    IEC1bits.U2RXIE = 0; // ���������� ���������� �� ��������� UART2 (RS-232)
    
    CNEN1bits.CN0IE = 0; //���������� ���������� �� ��������� ������ DET
    IEC1bits.CNIE = 0; //���������� ���������� �� ��������� ������ (����������)
}

uint8_t get_addr(void)
{
    return ADDRBAUDRATEbits.addr;
}

static uint8_t get_baudrate(void)
{
    return ADDRBAUDRATEbits.baudrate;
}