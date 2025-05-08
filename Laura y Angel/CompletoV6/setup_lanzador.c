/*
 * setup_lanzador.c
 *
 * Created: 06/05/2025 23:41:29
 *  Author: ASUS
 *
 *	aquí se hace el setup de los puertos (timers / entradas / salidas)
 */ 
#include "lanzador.h"

void setupFinalesCarrera();
void setupMotores();
void setupLed();
void setupTimerBase();

void setup(){
	
	cli();

	setupFinalesCarrera();
	
	setupMotores();

	setupLed();
	
	setupTimerBase();

	sei();
}

void setupFinalesCarrera(){
	
	// config switches como entradas 
	DDRB &= ~((1 << SW4) | (1 << SW3) | (1 << SW2));
	// habilitamos el grupo de interrupciones
	PCICR |= (1 << PCIE0);
	// selección pines interrupción
	PCMSK0 |= (1 << PCINT2) | (1 << PCINT3) | (1 << PCINT4);
}

void setupMotores(){
	
	// config direcciones como salidas 
	DDRL |= (1 << DIR4) | (1 << DIR3) | (1 << DIR2);
	// config enables como salidas
	DDRL |= (1 << EN4) | (1 << EN3) | (1 << EN2);
	
	// config TIMER5
	// config Fast PWM con ICR5 (1 1 1 0)
	TCCR5A &= ~(1 << WGM50);
	TCCR5A |= (1 << WGM51);
	TCCR5B |= (1 << WGM52);
	TCCR5B |= (1 << WGM53);
	// config prescaler (8)
	TCCR5B |= (1 << CS51);
	TCCR5B &= ~((1 << CS50) | (1 << CS52));
	// config velocidad PWM
	OCR5A = PWM2;
	OCR5B = PWM3;
	OCR5C = PWM4;
	// config del TOP
	ICR5 = 0xFFFF;
}

void setupLed(){
	DDRL |= (1 << LED);
}

void setupTimerBase(){
	// config CTC
	TCCR4A &= ~((1 << WGM41) | (1 << WGM40));
	TCCR4B &= ~(1 << WGM43);
	TCCR4B |= (1 << WGM42);
	// config prescaler (8)
	TCCR4B |= (1 << CS41);
	TCCR4B &= ~((1 << CS42) | (1 << CS40));
	// config valor interrupcion (1ms)
	OCR4A = 65535;
	// habilitar la interrupción por comparación
	TIMSK4 |= (1 << OCIE4A);
	// clear del contador
	TCNT4 = 0;
}