#include "main.h"
#include "init.h"

int main(void)
{
    init_modem();

    int_ON();
    RX_FX604;
    while (1);

    return (0);
}