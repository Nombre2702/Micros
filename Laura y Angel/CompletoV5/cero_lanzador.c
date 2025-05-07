/*
 * cero_lanzador.c
 *
 * Created: 07/05/2025 0:27:50
 *  Author: ASUS
 */ 

#include "lanzador.h"

#define NUM_PINES_REBOTES 3

volatile uint8_t init_cont_cero = 0;
volatile uint16_t cont_cero = 0;

volatile uint8_t init_cont_led = 0;
volatile uint8_t cont_led = 0;

volatile uint8_t estado_anterior = 0xFF;
volatile uint8_t estado_actual = 0;
volatile uint8_t flancos_detectados = 0;
volatile uint8_t debounce_activo = 0;
volatile uint16_t debounce_timer[NUM_PINES_REBOTES] = {0};
const uint8_t pines_rebotes[NUM_PINES_REBOTES] = {SW2, SW3, SW4};
	
// definimos las flags que seguramente haya que cambiar por callbacks
volatile uint8_t flag_cero_brazo = 0;

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
	
	if (init_cont_led == 1){cont_led++;}
	
	if (init_cont_cero == 1){cont_cero++;}
		
	for (uint8_t i = 0; i < NUM_PINES_REBOTES; i++) {
		uint8_t pin_bit = pines_rebotes[i];
		if (debounce_activo & (1 << pin_bit)) {
			debounce_timer[i]++;
			if (debounce_timer[i] >= 20) {
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
