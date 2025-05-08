/*
 * CompletoV6.c
 *
 * Created: 08/05/2025 14:42:34
 * Author : ASUS
 */ 

#include <avr/io.h>
#include "lanzador.h"

int main(void)
{

	setupLanzador();
	brazoParar();
    /* Replace with your application code */
    while (1) 
    {
		
		//ceroLanzador();
		//preparacionCabeceo();
		cabeceo();
		ledParpadeo();
    }
}

