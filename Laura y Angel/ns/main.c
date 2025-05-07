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





ISR(PCINT0_vect) {
	lectura2 = PINB;
	
	 if (PINB & (1 << PB2)) sw2 = 1;
	 if (PINB & (1 << PB3)) sw3 = 1;
	 if (PINB & (1 << PB4)) sw4 = 1;
	
}



void antirrebotes(){
	if (sw2) {
		flancobajada |= (1 << SW2);
		bloqueoentrada |= (1 << SW2);
		rebotesw2 = 0;
	}
	
	if (sw3) {
		flancobajada |= (1 << SW3);
		bloqueoentrada |= (1 << SW3);
		rebotesw3 = 0;
	}
	
	if (sw4) {
		flancobajada |= (1 << SW4);
		bloqueoentrada |= (1 << SW4);
		rebotesw4 = 0;
	}
	lectura1 = lectura2;
	
}

ISR(TIMER4_COMPA_vect){
	unMs();
	if (bloqueoentrada & (1 << SW2)) {
		rebotesw2++;
		if (rebotesw2 >= DEBOUNCE_TIEMPO_MS) {
			rebotesw2 = 0;
			bloqueoentrada &= ~(1 << SW2);
		}
	}

	if (bloqueoentrada & (1 << SW3)) {
		rebotesw3++;
		if (rebotesw3 >= DEBOUNCE_TIEMPO_MS) {
			rebotesw3 = 0;
			bloqueoentrada &= ~(1 << SW3);
		}
	}

	if (bloqueoentrada & (1 << SW4)) {
		rebotesw4++;
		if (rebotesw4 >= DEBOUNCE_TIEMPO_MS) {
			rebotesw4 = 0;
			bloqueoentrada &= ~(1 << SW4);
		}
	}
}
