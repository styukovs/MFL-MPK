#ifndef INIT_H
#define INIT_H

#include "common.h"

void init_modem(void);

void int_ON(void); //������������� ����������
void int_OFF(void); //���������� ����������

uint8_t get_addr(void); //������ ������


#endif //INIT_H