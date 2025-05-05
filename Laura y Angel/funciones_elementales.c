//FUNCIONES ELEMENTALES

#include <avr/io.h>
#include <avr/interrupt.h>
#include "funciones_elementales.h"



void pararPlataforma(){
	OCR5A = 0;
}

void rotarIzquierda(){
	// configurar dirección

	PORTL |= (1 << PL0);
	TCCR5A |= (1 << COM5A1);
	TCCR5A &= ~(1 << COM5A0);

}

void rotarDerecha(){
	// configurar dirección
	
	PORTL &= ~(1 << PL0);
	TCCR5A |= (1 << COM5A1);
	TCCR5A &= ~(1 << COM5A0);

	
}

void ceroPlataforma(int contador){
	// se puede hacer algo con los finales de carrera
	// habría que medir tiempos antes
	if (contador == 2){
		pararPlataforma();
		}else{
		rotarDerecha();
	}
}

void moverAdelante(){
	// configurar dirección

	PORTL &= ~(1 << PL1); //dirección
	TCCR5B |= (1 << COM5A1);
	TCCR5B &= ~(1 << COM5A0);

}

void moverAtras(int duty){
	// configurar dirección

	PORTL |= (1 << PL1); //dirección
	TCCR5B |= (1 << COM5A1);
	TCCR5B &= ~(1 << COM5A0);

}

void pararVastago(){
	OCR5B = 0;
}

void ceroVastago(int contador){
	// se mueve hasta llegar alante del todo
	// falta determinar el tiempo y la escala temporal
	if(contador == 2){
		pararVastago();
		}else{
		moverAdelante();
	}
}

void barrera_arriba(){
	//REVISAR
	PORTL |= (1 << PL2);
	TCCR5C |= (1 << COM5A1);
	TCCR5C &= ~(1 << COM5A0);
	
}


void barrera_abajo(){
	//REVISAR

	PORTL &= ~(1 << PL2); //bajar barrera
	TCCR5C |= (1 << COM5A1);
	TCCR5C &= ~(1 << COM5A0);
}

void origen_barrera(){
	barrera_arriba(); // esto a mi me sobra porque con subir barrera y las interrupciones habilitadas
	//se puede hacer
}

void led_on(){
	PORTL |= (1 << PL6);
}

// FUNCIONES LED
void led_off(){
	PORTL &= ~(1 << PL6);
}

void parpadeo(){
	led_off();
	//timer gestionar con interrupción? o tengo que hacer yo el contador?
	led_on();
}


void giro(int sentido_giro){
	if (sentido_giro){
		rotarIzquierda(); //comprobar porque me lo acabo de inventar y puede ser derecha
	}
	else rotarDerecha();
}
