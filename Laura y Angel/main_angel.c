#include <avr/io.h>
#include <avr/interrupt.h>
#include "micros_funciones_angel.h"

int main(void)
{
	
	setupLanzador();
	setupMillis();

    while (1) 
    {
		balanceoLanzador();
    }
}

