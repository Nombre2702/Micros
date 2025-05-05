#include <avr/io.h>
#include <avr/interrupt.h>
#include "micros_funciones_angel.h"

volatile uint8_t contador_sw2 = 0;
volatile uint8_t contador_sw3 = 0;
volatile uint8_t contador_sw4 = 0;
volatile uint8_t sentido_plataforma = 0;

volatile uint32_t timer1_millis = 0;

volatile uint32_t last_time_sw2 = 0;
volatile uint32_t last_time_sw3 = 0;
volatile uint32_t last_time_sw4 = 0;

int main(void)
{
	
	setupLanzador();
	setupMillis();

    while (1) 
    {
		balanceoLanzador();
    }
}

