#include "micros_funciones_angel.h"

volatile uint8_t contador_aux = 0;
volatile uint8_t contador_sw2 = 0;
volatile uint8_t contador_sw3 = 0;
volatile uint8_t contador_sw4 = 0;
volatile uint8_t sentido_plataforma = 0;
volatile uint16_t duty_2 = 50000;
volatile uint16_t duty_3 = 50000;
volatile uint16_t duty_4 = 50000;

volatile uint32_t timer1_millis = 0;

volatile uint32_t last_time_sw2 = 0;
volatile uint32_t last_time_sw3 = 0;
volatile uint32_t last_time_sw4 = 0;

ISR(PCINT2_vect){
	uint32_t now = millis();

	// !(PINK & (1 << PKn)) devuelve un 1 (es decir que entra en el if) si el pin está a 0
	if (!(PINK & (1 << PK0)) && (now - last_time_sw2 > DEBOUNCE_TIME)) {
		last_time_sw2 = now;
		contador_sw2++;
	}
	if (!(PINK & (1 << PK1)) && (now - last_time_sw3 > DEBOUNCE_TIME)) {
		last_time_sw3 = now;
		contador_sw3++;
	}
	if (!(PINK & (1 << PK2)) && (now - last_time_sw4 > DEBOUNCE_TIME)) {
		last_time_sw4 = now;
		contador_sw4++;
	}
}

ISR(TIMER1_COMPA_vect){
	timer1_millis++;
}

uint32_t millis(){
	uint32_t ms;
	cli();
	ms = timer1_millis;
	sei();
	return ms;
}

void setupMillis(){
	TCCR1A = 0;
	TCCR1B = 0;

	TCCR1B |= (1 << WGM12); // CTC con OCR1A

	TCCR1B |= (1 << CS11) | (1 << CS10); // preescalado de 8, por comodidad

	OCR1A = 999; // valor en el que parar de contar (ajustado a 1 ms)

	TIMSK1 |= (1 << OCIE1A); // habilita la interrupción por comparación

	TCNT1 = 0; 
}


void setupLanzador(){
	// deshabilitamos las interrupciones globales
	cli();
	
	// FINALES DE CARRERA / INTERRUPCIONES
	// config entradas switches | puerto K
	// K0 = sw2, K1 = sw3, K2 = sw4
	DDRK &= ~((1 << PK2) | (1 << PK1) | (1 << PK0));
	// habilitamos el grupo de interrupciones
	PCICR |= (1 << PCIE2); 
	// seleccion pines(K0 = PCINT16, K1 = PCINT17, K2 = PCINT18)
	PCMSK2 |= (1 << PCINT16) | (1 << PCINT17) | (1 << PCINT18);
	
	// DIRECCIONES
	// config salidas direcciones | puerto L
	DDRL |= (1 << PL2) | (1 << PL1) | (1 << PL0);
	
	// ENABLES
	// setup del timer 5 | puerto L | Fast PWM
	// config salidas puerto L con enmascaramiento
	DDRL |= (1 << PL5) | (1 << PL4) | (1 << PL3);
	
	// config Fast PWM con ICR5 (1 1 1 0)
	TCCR5A &= ~(1 << WGM50);
	TCCR5A |= (1 << WGM51);
	TCCR5B |= (1 << WGM52);
	TCCR5B |= (1 << WGM53);
	
	// config prescaler (8)
	TCCR5B |= (1 << CS51);
	TCCR5B &= ~((1 << CS50) | (1 << CS52));
	
	// config PWM
	OCR5A = duty_2;
	OCR5B = duty_3;
	OCR5C = duty_4;
	
	// config del TOP | ICR5 | es un registro de 16 bits asi que para aprovecharlo al maximo lo ponemos a 0xFFFF
	ICR5 = 0xFFFF;
	
	// volvemos a habilitar las interrupciones globales
	sei();
}

// FUNCIONES PLATAFORMA | motor 2 | sw 2

void pararPlataforma(){
	TCCR5A &= ~(1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
}

void rotarIzquierda(){
	// configurar dirección
	PORTL |= (1 << PL0);
	// duty nuevo: se configura con el COM
	TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
}

void rotarDerecha(){
	// configurar dirección
	PORTL &= ~(1 << PL0);
	// duty nuevo
	TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
}

void ceroPlataforma(){
	// se puede hacer algo con los finales de carrera
	// habría que medir tiempos antes
	if (contador_aux == 2){
		pararPlataforma();
		contador_aux = 0;
	}else{
		rotarDerecha();
	}
}


void balanceoLanzador(){
	if (contador_sw2 >= 3) {
		pararPlataforma();
		if (sentido_plataforma == 0) {
			rotarDerecha();
		} else {
			rotarIzquierda();
		}
		sentido_plataforma = !sentido_plataforma;
		contador_sw2 = 0;
	} else {
		if (sentido_plataforma == 0) {
			rotarIzquierda();
		} else {
			rotarDerecha();
		}
	}
}
// FUNCIONES VÁSTAGO

void moverAdelante(){
	// configurar dirección
	PORTL |= (1 << PL1);
	// duty nuevo
	TCCR5A |= (1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
}

void moverAtras(){
	// configurar dirección
	PORTL &= ~(1 << PL1);
	// duty nuevo
	TCCR5A |= (1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
}

void pararVastago(){
	TCCR5A &= ~(1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
}

void ceroVastago(){
	// se mueve hasta llegar alante del todo
	// falta determinar el tiempo y la escala temporal
	if(contador_aux == 2){
		pararVastago();
		contador_aux = 0;
	}else{
		moverAdelante();
	}
}

void disparo(){
	// el duty que le pases será la fuerza con la que tire
	// se puede meter algún tipo de seguridad interna en la función
	// no la pongo para que les sea más fácil a los de integración
	
	// init
	pararPlataforma();
	pararVastago();
	
	// lógica
	// lanza hasta llegar al final que para
	if(contador_sw3 == 1){
		moverAdelante();
	}
	else if(contador_sw3 == 2){
		pararVastago();
		contador_sw3 = 0;
	}
}