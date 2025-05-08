/*
 * funciones_movimiento.c
 *
 * Created: 06/05/2025 23:40:26
 *  Author: ASUS
 *	
 *	aqui se definen las funciones de más bajo nivel
 */ 

#include "lanzador.h"

// BRAZO
void brazoDerecha(){
	// config direccion
	PORTL &= ~(1 << DIR2);
	// habilitar PWM
	TCCR5A |= (1 << COM5A1);
	TCCR5A &= ~(1 << COM5A0);
}

void brazoIzquierda(){
	// config direccion
	PORTL |= (1 << DIR2);
	// habilitar PWM
	TCCR5A |= (1 << COM5A1);
	TCCR5A &= ~(1 << COM5A0);
}

void brazoParar(){
	// deshabilitar PWM
	TCCR5A &= ~(1 << COM5A1);
	TCCR5A &= ~(1 << COM5A0);
}

// VÁSTAGO
void vastagoArriba(){
	// config direccion
	PORTL |= (1 << DIR3);
	// habilitar PWM
	TCCR5A |= (1 << COM5B1);
	TCCR5A &= ~(1 << COM5B0);
}

void vastagoAbajo(){
	// config direccion
	PORTL &= ~(1 << DIR3);
	// habilitar PWM
	TCCR5A |= (1 << COM5B1);
	TCCR5A &= ~(1 << COM5B0);
}

void vastagoParar(){
	// deshabilitar PWM
	TCCR5A &= ~(1 << COM5B1);
	TCCR5A &= ~(1 << COM5B0);
}

// BARRERA
void barreraArriba(){
	// config direccion
	PORTL |= (1 << DIR4);
	// habilitar PWM
	TCCR5A |= (1 << COM5C1);
	TCCR5A &= ~(1 << COM5C0);
}

void barreraAbajo(){
	// config direccion
	PORTL &= ~(1 << DIR4);
	// habilitar PWM
	TCCR5A |= (1 << COM5C1);
	TCCR5A &= ~(1 << COM5C0);
}

void barreraParar(){
	// deshabilitar PWM
	TCCR5A &= ~(1 << COM5C1);
	TCCR5A &= ~(1 << COM5C0);
}

// LED
void ledOn(){
	PORTL |= (1 << LED);
}

void ledOff(){
	PORTL &= ~(1 << LED);
	init_cont_led = 0;
}

void ledParpadeo(){
	init_cont_led = 1;
	if(cont_led < 500){
		ledOn();
	}
	else if((cont_led < 1000) && (cont_led > 500)){
		ledOff();
	}else{
		cont_led = 0;
	}
}
