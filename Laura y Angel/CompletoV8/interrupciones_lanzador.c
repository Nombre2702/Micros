/*
 * interrupciones_lanzador.c
 *
 * Created: 08/05/2025 17:57:21
 *  Author: ASUS
 */ 
#include "lanzador.h"

// VARIABLES LED
volatile uint8_t init_cont_led = 1;
volatile uint16_t cont_led = 0;

// VARIABLES CERO
volatile uint8_t init_cont_cero = 0;
volatile uint16_t cont_cero = 0;

// VARIABLES DEBOUNCER
volatile uint8_t estado_anterior = 0xFF;
volatile uint8_t estado_actual = 0;
volatile uint8_t flancos_detectados = 0;
volatile uint8_t debounce_activo = 0;
volatile uint16_t debounce_timer[NUM_PINES_REBOTES] = {0};
const uint8_t pines_rebotes[NUM_PINES_REBOTES] = {SW2, SW3, SW4};

// INTERRUPCIONES
ISR(PCINT0_vect){

	estado_actual = PINB;

	antirrebotesLanzador(SW2);
	antirrebotesLanzador(SW3);
	antirrebotesLanzador(SW4);
	
	estado_anterior = estado_actual;
}

ISR(TIMER4_COMPA_vect){
	millisLanzador();
}

// CONTADOR 1 MS
void millisLanzador(){
	if(init_cont_led == 1){ cont_led = cont_led + 1; }
		
	if(init_cont_cero == 1){cont_cero++;}
	
	for (uint8_t i = 0; i < NUM_PINES_REBOTES; i++) {
		uint8_t pin_bit = pines_rebotes[i];
		if (debounce_activo & (1 << pin_bit)) {
			debounce_timer[i]++;
			if (debounce_timer[i] >= 5) {
				debounce_activo &= ~(1 << pin_bit);
				debounce_timer[i] = 0;
			}
		}
	}
}

// ANTIRREBOTES
void antirrebotesLanzador(uint8_t pin_bit) {
	for (uint8_t i = 0; i < NUM_PINES_REBOTES; i++) {
		if (pines_rebotes[i] == pin_bit) {
			if (!(estado_actual & (1 << pin_bit)) && (estado_anterior & (1 << pin_bit)) && !(debounce_activo & (1 << pin_bit))) {
				flancos_detectados |= (1 << pin_bit);
				debounce_activo |= (1 << pin_bit);
				debounce_timer[i] = 0;
			}
			break;
		}
	}
}

// LED
void ledParpadeoOff(){
	ledOff();
	init_cont_led = 0;
}

void ledParpadeoOn(){
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

// PUESTA A CERO
void ceroLanzador(){
	
	init_cont_cero = 1;
	
	if (cont_cero < 2000){
		brazoDerecha();
		barreraAbajo();
		
		if(flancos_detectados & (1 << SW4)){
			barreraParar();
			vastagoArriba();
			flancos_detectados &= ~(1 << SW4);
			
			if(flancos_detectados & (1 << SW3)){
				vastagoParar();
				flancos_detectados &= ~(1 << SW3);
			}
		}
		
		} else {
		brazoParar();
		barreraParar();
		vastagoParar();
		
		flancos_detectados &= ~((1 << SW4) | (1 << SW3) | (1 << SW2));
		
		cont_cero = 0;
		init_cont_cero = 0;
		//flag_cero_brazo = 1; // esto hay que cambiarlo por un callback, creo
	}
}
