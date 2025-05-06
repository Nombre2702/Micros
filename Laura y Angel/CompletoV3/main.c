#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "funciones_lanzador.h"

int main(void)
{
	
	setupLanzador();
	setupMillis();
	
	ceroPlataforma();
	ledOn();

    while (1) 
    {
		
    }
}

