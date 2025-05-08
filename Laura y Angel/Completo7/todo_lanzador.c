/*
 * todo_lanzador.c
 *
 * Created: 08/05/2025 14:56:39
 *  Author: ASUS
 */ 

#include "lanzador.h"

#define NUM_PINES_REBOTES 3

volatile uint8_t init_cont_cero = 0;
volatile uint16_t cont_cero = 0;

volatile uint8_t init_cont_led = 0;
volatile uint16_t cont_led = 0;

volatile uint8_t direccion = 0;
volatile uint8_t iniciado = 0;

volatile uint8_t estado_anterior = 0xFF;
volatile uint8_t estado_actual = 0;
volatile uint8_t flancos_detectados = 0;
volatile uint8_t debounce_activo = 0;
volatile uint16_t debounce_timer[NUM_PINES_REBOTES] = {0};
const uint8_t pines_rebotes[NUM_PINES_REBOTES] = {SW2, SW3, SW4};

// definimos las flags que seguramente haya que cambiar por callbacks
volatile uint8_t flag_cero_brazo = 0;


// definimos las flags que seguramente haya que cambiar por callbacks
volatile uint8_t flag_preparado_cabeceo = 0;
volatile uint8_t estado_cabeceo = 0;
volatile uint8_t flag_preparado_carga = 0;
volatile uint8_t flag_disparo_listo = 0;
volatile uint8_t flag_vastago_cero = 0; // esta es externa

/*
*	mientras movemos hacia la derecha (va a ser un tiempo fijo de 2 segundos)
*	subimos la barrera
*	una vez verficado que está subida
*	subimos/adelantamos el vastago
*/

void millisLanzador();
void antirrebotesLanzador(uint8_t pin_bit);

ISR(PCINT0_vect){

	estado_actual = PINB;

	antirrebotesLanzador(SW2);
	antirrebotesLanzador(SW3);
	antirrebotesLanzador(SW4);
	
	estado_anterior = estado_actual;
}

ISR(TIMER4_COMPA_vect){
	// esta interrupción salta cada milisegundo
	millisLanzador();
}

void millisLanzador(){
	
	if (init_cont_led == 1){cont_led+=cont_led;} 
	
	if (init_cont_cero == 1){cont_cero++;} 
	
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
				flag_vastago_cero = 1;
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
		flag_cero_brazo = 1; // esto hay que cambiarlo por un callback, creo
	}
}

/*
Con el antirrebotes una vez detectamos el flanco de bajada dejamos de detectar durante 20 ms más flancos en ese pin
Pero no se limpia en ningún momento
Por ello tenemos que limpiarlo después de usarlo, para que no de errores con el resto del código
*/

// PREPARACION CABECEO
// preparacionCabeceo solo se ejecuta si estamos a cero y el retorno nos deja pasar
// tenemos que decir que hemos terminado este estado, con otra flag mas
// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)

void preparacionCabeceo(){
	if(flag_preparado_cabeceo == 0){
		brazoIzquierda();
	}
	if(flancos_detectados & (1 << SW2)){
		brazoParar();
		flancos_detectados &= ~(1 << SW2);
		
		flag_preparado_cabeceo = 1;
	}
}

// CABECEO
// cabeceo solo se ejecuta si estamos preparados
// tengo que decir que estoy en este estado, con otro flag supongo
// sabemos que partimos del medio
// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)
// cuando lo detecte cambiamos de direccion (se me ocurre con hacerlo con una variable direccion y un if)

void cabeceo(){
	estado_cabeceo = 1;

	if (!iniciado) {
		iniciado = 1;
		brazoIzquierda();
	}

	if (flancos_detectados & (1 << SW2)) {
		flancos_detectados &= ~(1 << SW2);

		if (direccion == 0) {
			brazoParar();
			brazoDerecha();
			direccion = 1;
		} else {
			brazoParar();
			brazoIzquierda();
			direccion = 0;
		}
	}
}

// CARGA LANZADOR
// se va hasta la izquierda del todo, sabemos que vamos a estar en algún punto intermedio así que solo trenemos que esperar a un flanco de bajada de SW2
// luego se mueve hacia la derecha hasta que se detectan 2 flancos de bajada (uno en el medio y otro al llegar ambos de SW2)

void prepararCarga(){
	static uint8_t estado = 0;
	static uint8_t flancos_contados = 0;
	
	vastagoArriba();
	if(flancos_detectados & (1 << SW3)){
		vastagoParar();
		flancos_detectados &= ~(1 << SW3);
	}

	switch (estado) {
		case 0:
		brazoIzquierda();
		if (flancos_detectados & (1 << SW2)) {
			flancos_detectados &= ~(1 << SW2);
			brazoParar();
			estado = 1;
			flancos_contados = 0;
		}
		break;

		case 1:
		brazoDerecha();
		if (flancos_detectados & (1 << SW2)) {
			flancos_detectados &= ~(1 << SW2);
			flancos_contados++;
			if (flancos_contados >= 2) {
				brazoParar();
				flag_preparado_carga = 1;
				estado = 0;
			}
		}
		break;
	}
}

// CARGA
// tiene que subir la barrera para encganchar la retencion y luego bajar el vastago para tensar las gomas
void cargarDisparo(){
	if (flag_vastago_cero == 1){
		barreraArriba();
		
		if(flancos_detectados & (1 << SW4)){
			barreraParar();
			vastagoAbajo();
			flancos_detectados &= ~(1 << SW4);

			if(flancos_detectados & (1 << SW3)){
				vastagoParar();
				flag_disparo_listo = 1;
				flancos_detectados &= ~(1 << SW3);
			}
		}
	}

	if (flag_disparo_listo == 1){
		ledOn();
	}
}

// DISPARO
// lo único que tiene que hacer es liberar la retención, no toca el vástago!!
void disparo(){
	brazoParar();
	ledOff();
	barreraAbajo();
	
	if (flancos_detectados & (1 << SW4)){
		barreraParar();
		flancos_detectados &= ~(1 << SW4);
	}
}

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
	if((cont_led < 1000) && (cont_led >= 500)){
		ledOff();
	}
	if(cont_led >= 1000){
		cont_led = 0;
	}
}


/*
void ledParpadeo(){
	
	if (cont_led == 500){
		PORTL ^= (1 << LED);
		cont_led = 0;
	}
}
*/