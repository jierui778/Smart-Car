#include "buzzer.h"
void Buzzer_Init()
{
    gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL);
}

void Buzzer_On()
{
    gpio_set_level(BEEP, GPIO_HIGH);
}


void Buzzer_Off()
{
    gpio_set_level(BEEP, GPIO_LOW);
}