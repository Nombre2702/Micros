#include <avr/io.h>
#include <avr/interrupt.h>
#include "funciones_lanzador.h"

int main(void)
{
	
	setupLanzador();
	setupMillis();

    while (1) 
    {
		balanceoLanzador();
    }
}

