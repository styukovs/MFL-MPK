#ifndef INIT_H
#define INIT_H

#include "common.h"

void init_modem(void);

void int_ON(void); //инициализация прерываний
void int_OFF(void); //отключение прерываний

uint8_t get_addr(void); //взятие адреса


#endif //INIT_H