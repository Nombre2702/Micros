#include "funciones_lanzador.h"

volatile int comienzo = 0;
volatile int cuenta_comienzo = 0;

volatile uint8_t contador_cero_plat = 0;
volatile uint8_t contador_cero_vast = 0;
volatile uint8_t contador_cero_barr = 0;
volatile uint8_t contador_parpadeo_led = 0;

volatile uint8_t contador_sw2 = 0;
volatile uint8_t contador_sw3 = 0;
volatile uint8_t contador_sw4 = 0;
volatile uint8_t sentido_plataforma = 0;

volatile uint32_t timer4_millis = 0;

volatile uint32_t last_time_sw2 = 0;
volatile uint32_t last_time_sw3 = 0;
volatile uint32_t last_time_sw4 = 0;

ISR(PCINT2_vect){
	uint32_t now = millis();

	// !(PINK & (1 << PKn)) devuelve un 1 (es decir que entra en el if) si el pin está a 0
	if (!(PINK & (1 << SW2)) && (now - last_time_sw2 > DEBOUNCE_TIME)) {
		last_time_sw2 = now;
		contador_sw2++;
	}
	if (!(PINK & (1 << SW3)) && (now - last_time_sw3 > DEBOUNCE_TIME)) {
		last_time_sw3 = now;
		contador_sw3++;
	}
	if (!(PINK & (1 << SW4)) && (now - last_time_sw4 > DEBOUNCE_TIME)) {
		last_time_sw4 = now;
		contador_sw4++;
	}
}

ISR(TIMER4_COMPA_vect){
	unMs();
}

void unMs(){
	
	if(comienzo == 1) {
		
		cuenta_comienzo++;
		
	}
	
}

uint32_t millis(){
	volatile uint32_t ms;
	
	cli();
	ms = timer4_millis;
	sei();
	
	return ms;
}

void setupMillis(){
	cli();
	
	// modo CTC
	TCCR4B |= (1 << WGM42);
	// preescalado de 8
	TCCR4B |= (1 << CS41);
	TCCR4B &= ~((1 << CS42) | (1 << CS40));

	// valor en el que parar de contar (ajustado a 1 ms)
	OCR4A = 999; 

	// habilita la interrupción por comparación
	TIMSK4 |= (1 << OCIE4A);

	TCNT4 = 0; 
	
	sei();
}


void setupLanzador(){
	// deshabilitamos las interrupciones globales
	cli();
	
	// FINALES DE CARRERA / INTERRUPCIONES
	// config entradas switches
	DDRB &= ~((1 << SW4) | (1 << SW3) | (1 << SW2));
	// habilitamos el grupo de interrupciones
	PCICR |= (1 << PCIE0); 
	// selección pines interrupción
	PCMSK2 |= (1 << PCINT2) | (1 << PCINT3) | (1 << PCINT4);
	
	// DIRECCIONES
	// config salidas direcciones | puerto L
	DDRL |= (1 << DIR4) | (1 << DIR3) | (1 << PL0);
	
	// ENABLES
	// setup del timer 5 | puerto L | Fast PWM
	// config salidas puerto L con enmascaramiento
	DDRL |= (1 << PL6) | (1 << PL5) | (1 << PL4) | (1 << PL3);
	
	// config Fast PWM con ICR5 (1 1 1 0)
	TCCR5A &= ~(1 << WGM50);
	TCCR5A |= (1 << WGM51);
	TCCR5B |= (1 << WGM52);
	TCCR5B |= (1 << WGM53);
	
	// config prescaler (8)
	TCCR5B |= (1 << CS51);
	TCCR5B &= ~((1 << CS50) | (1 << CS52));
	
	// config PWM
	OCR5A = PWM2;
	OCR5B = PWM3;
	OCR5C = PWM4;
	
	// config del TOP | ICR5 | es un registro de 16 bits asi que para aprovecharlo al maximo lo ponemos a 0xFFFF
	ICR5 = 0xFFFF;
	
	// config LED
	DDRL |= (1 << PL6);
	
	// volvemos a habilitar las interrupciones globales
	sei();
}

// FUNCIONES PLATAFORMA | motor 2 | sw 2

void pararPlataforma(){
	TCCR5A &= ~(1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
}

void rotarIzquierda(){
	// configurar dirección
	PORTL &= ~(1 << DIR2);
	// duty nuevo: se configura con el COM
	TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
}

void rotarDerecha(){
	// configurar dirección
	PORTL |= (1 << DIR2);
	// duty nuevo
	TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
}

void ceroPlataforma(){

	comienzo == 1;
	
	if (cuenta_comienzo < 2000){
		rotarDerecha();
		comienzo = 0;
		cuenta_comienzo = 0;
		} else {
		pararPlataforma();
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
	if(contador_cero_vast == 1000){
		pararVastago();
		contador_cero_vast = 0;
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

// FUNCIONES BARRERA

void barreraArriba(){

	PORTL |= (1 << PL2);
	TCCR5A |= (1 << COM5C1);
	TCCR5A &= ~(1 << COM5C0);
	
}

void barreraAbajo(){

	PORTL &= ~(1 << PL2); //bajar barrera
	TCCR5A |= (1 << COM5C1);
	TCCR5A &= ~(1 << COM5C0);
}

void origenBarrera(){
	
	barreraArriba();
	
}

// FUNCIONES LED

void ledOn(){
	
	PORTL |= (1 << PL6);
	
}

void ledOff(){
	
	PORTL &= ~(1 << PL6);
	
}

void ledParpadeo(){
	
	ledOff();
	
	ledOn();
	
}