/*
 * bolos.c
 *
 * Created: 12/05/2025 17:37:10
 *  Author: ASUS
 */ 

#include "lanzador.h"
#include "integracion.h"

//BOLOS

#define NUM_BOLOS 6  //Numero de bolos

// Variables para almacenar si un bolo fue golpeado alguna vez (1)->GOLPEADO

volatile uint8_t bolo_golpeado[NUM_BOLOS] = {0};

// Estado anterior de PK0–PK5 (solo flanco descendente)

volatile uint8_t estado_anterior_bolos = 0x3F;  // Todos en reposo (1) -> 00111111

// Puntuación acumulada (Suma de tiradas)

volatile uint8_t puntaje_total = 0;

void init_bolos() {
	
	cli();
	DDRK &= ~((1 << PK5) | (1 << PK4) | (1 << PK3) | (1 << PK2) | (1 << PK1) | (1 << PK0));  // Entradas
	PCICR |= (1 << PCIE2);  // Grupo de interrupción
	PCMSK2 |= 0b00111111;  // Pines PK0–PK5
	sei();  // Habilitar interrupciones globales
}


// ISR para detectar flanco descendente en PK0–PK5

ISR(PCINT2_vect) {
	
	//Deja activo solo el bit del PIN que se haya movido
	
	uint8_t estado_actual = PINK & 0x3F;
	
	for (uint8_t i = 0; i < NUM_BOLOS; i++) {
		
		//Creación de una mascara de todo 1 para comparar bit a bit con el estado actual y anterior
		
		uint8_t m = (1<<i);
		
		//Comparaciones para dejar a '1' el i del bolo golpeado
		
		if ((estado_anterior_bolos & m) && !(estado_actual & m)) {
			
			//Vemos si el bolo golpeado ya fue contado
			if (!bolo_golpeado[i]) {
				bolo_golpeado[i] = 1;
				puntaje_total++;
			}
		}
	}
	
	estado_anterior_bolos = estado_actual;
}

// Devuelve el puntaje total acumulado

uint8_t obtener_puntaje_total() {
	return puntaje_total;
}

// Sólo limpia el estado de los bolos para el siguiente lanzamiento

void reiniciar_conteo() {
	for (uint8_t i = 0; i < NUM_BOLOS; i++) {
		bolo_golpeado[i] = 0;
	}
	estado_anterior_bolos = PINK & 0x3F;
	
	return;
}

// Reinicia TODO el juego (puntaje acumulado y bolos)

void reiniciar_juego() {
	reiniciar_conteo();
	puntaje_total = 0;
}