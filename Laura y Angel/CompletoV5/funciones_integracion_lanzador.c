/*
 * funciones_integracion.c
 *
 * Created: 06/05/2025 23:41:48
 *  Author: ASUS
 *
 *	Aqu� se incluyen las funciones que usar� el equipo de integraci�n
 */ 

#include "lanzador.h"

volatile uint8_t flancos_detectados;


// definimos las flags que seguramente haya que cambiar por callbacks
volatile uint8_t flag_preparado_cabeceo = 0;
volatile uint8_t estado_cabeceo = 0;
volatile uint8_t flag_preparado_carga = 0;
volatile uint8_t flag_disparo_listo = 0;
volatile uint8_t flag_vastago_cero; // esta es externa

// PREPARACION CABECEO 
// preparacionCabeceo solo se ejecuta si estamos a cero y el retorno nos deja pasar
// tenemos que decir que hemos terminado este estado, con otra flag mas
	// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)

void preparacionCabeceo(){
	brazoIzquierda();
	
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
	
	static uint8_t direccion = 0;
	static uint8_t iniciado = 0;

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
	// se va hasta la izquierda del todo, sabemos que vamos a estar en alg�n punto intermedio as� que solo trenemos que esperar a un flanco de bajada de SW2
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
	// lo �nico que tiene que hacer es liberar la retenci�n, no toca el v�stago!!
void disparo(){
	brazoParar();
	ledOff();
	barreraAbajo();
	
	if (flancos_detectados & (1 << SW4)){
		barreraParar();
		flancos_detectados &= ~(1 << SW4);
	}
}