/*
 * funciones_integracion.c
 *
 * Created: 06/05/2025 23:41:48
 *  Author: ASUS
 *
 *	Aquí se incluyen las funciones que usará el equipo de integración
 */ 

#include "lanzador.h"

// FLAGS FINALES DE CARRERA
volatile uint8_t flancos_detectados;

// FLAGS CABECEO
volatile uint8_t direccion = 0;
volatile uint8_t iniciado = 0;

// FLAGS COMUNICACION
volatile uint8_t flag_disparo_listo = 0;

// REINICIO FLAGS 

void setupFlagsLanzador(){
	direccion = 0;
	iniciado = 0;
	flag_disparo_listo = 0;
}

// PREPARACION CABECEO 
// preparacionCabeceo solo se ejecuta si estamos a cero y el retorno nos deja pasar
// tenemos que decir que hemos terminado este estado, con otra flag mas
	// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)

void prepararCabeceo(void (*callback)(void)){
	
	brazoIzquierda();
	
	if(flancos_detectados & (1 << SW2)){
		brazoParar();
		flancos_detectados &= ~(1 << SW2);
		callback();
	}
}

// CABECEO
// cabeceo solo se ejecuta si estamos preparados
// tengo que decir que estoy en este estado, con otro flag supongo
	// sabemos que partimos del medio
	// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)
	// cuando lo detecte cambiamos de direccion (se me ocurre con hacerlo con una variable direccion y un if)

void cabeceo(){

	if (!iniciado) {
		brazoIzquierda();
		direccion = 0;
		iniciado = 1;
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

void prepararCarga(void (*callback)(void)){
	static uint8_t estado = 0;
	static uint8_t flancos_contados = 0;
	
	vastagoArriba();
	if(flancos_detectados & (1 << SW3)){
		flancos_detectados &= ~(1 << SW3);
		vastagoParar();
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
				estado = 0;
				callback();
			}
		}
		break;
	}
}

// CARGA 
	// tiene que subir la barrera para encganchar la retencion y luego bajar el vastago para tensar las gomas
void cargarDisparo(void (*callback)(void)){
	
	barreraArriba();
	
	if(flancos_detectados & (1 << SW4)){
		flancos_detectados &= ~(1 << SW4);
		barreraParar();
		vastagoAbajo();

		if(flancos_detectados & (1 << SW3)){
			flancos_detectados &= ~(1 << SW3);
			vastagoParar();
			flag_disparo_listo = 1;
			callback();
		}
	}
	

	if (flag_disparo_listo == 1){
		ledOn();
	}
}

// DISPARO
	// lo único que tiene que hacer es liberar la retención, no toca el vástago!!
void disparo(void (*callback)(void)){
	flag_disparo_listo = 0;
	
	brazoParar();
	ledOff();
	barreraAbajo();
	
	if (flancos_detectados & (1 << SW4)){
		flancos_detectados &= ~(1 << SW4);
		barreraParar();
		callback();
	}	
}