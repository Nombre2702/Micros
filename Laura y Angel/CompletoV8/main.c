/*
 * CompletoV8.c
 *
 * Created: 08/05/2025 17:55:28
 * Author : ASUS
 */ 

#include <avr/io.h>
#include "lanzador.h"


int main(void)
{
	setupLanzador();
	ledOn();
    /* Replace with your application code */
    while (1) 
    {
		barreraArriba();
		//ceroLanzador();
		//prepararCabeceo();
		//ledParpadeoOn();
		//cabeceo();
    }
}

