/*
 * parpadeoLed.c
 *
 * Created: 08/05/2025 15:25:40
 *  Author: ASUS
 */ 

#include "parpadeoLed.h"

volatile uint8_t init_cont_led = 1;
volatile uint16_t cont_led = 0;

ISR(TIMER4_COMPA_vect){
	millisLanzador();
}

void millisLanzador(){
	cont_led = cont_led + 1; 
}

void parpadeoLed(){
	init_cont_led = 1;
	if (cont_led < 500) {
		ledOn();
	}
	if (cont_led >= 500 && cont_led < 1000) {
		ledOff();
	}
	if (cont_led >= 1000) {
		cont_led = 0;
	}
}

void ledOn(){
	PORTL |= (1 << LED);
}

void ledOff(){
	PORTL &= ~(1 << LED);
}

void setup(){
	cli();
	
	DDRL |= (1 << LED);
	
	// config CTC
	TCCR4A &= ~((1 << WGM41) | (1 << WGM40));
	TCCR4B &= ~(1 << WGM43);
	TCCR4B |= (1 << WGM42);
	// config prescaler (8)
	TCCR4B |= (1 << CS41);
	TCCR4B &= ~((1 << CS42) | (1 << CS40));
	// config valor interrupcion (1ms)
	OCR4A = 999;
	// habilitar la interrupción por comparación
	TIMSK4 |= (1 << OCIE4A);
	// clear del contador
	TCNT4 = 0;
	
	sei();
}

