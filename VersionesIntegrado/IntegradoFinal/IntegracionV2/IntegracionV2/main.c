#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdbool.h>

#include "lanzador.h"
#include "carga_retorno.h"
#include "bolos.h"

#include "display.h"
#define T_PARTIDA 30000
#define T_TIMEOUT 3000
#define T_PISTA 3500
#define T_RETORNO 6500

// VER NUEVA

void boton_main();

typedef enum {
	JUEGO,
	JUEGO_FINAL,
	STBY
} juego;

typedef enum {
	INICIO,
	P_CERO,
	CARGA,
	CABECEO,
	DISPARO,
	RETORNO
} estado;


int temp_global;
int temp_sincro;
int temp_partida;

uint8_t puntuacion;
uint8_t puntuacion_display;

estado estado_maq;
juego estado_juego;

bool a_cero_lanzador;

bool a_cero_retorno;
bool a_cero_carga;

bool ha_cargado;
bool ha_disparado;

bool ha_subido;
bool ha_calculado;

bool ha_retornado;


void check_ha_cargado() { ha_cargado =  true; }
void check_ha_disparado() { ha_disparado = true; }

void check_ha_subido() { ha_subido = true; }
void check_cero_carga() { a_cero_carga = true; }
void check_cero_retorno() { a_cero_retorno = true; }

typedef enum {
	POS_PRE_CERO,
	POS_CAB,
	LISTO_PREVIO_CABECEO,
	LISTO_CABECEO,
	POS_CARGAR
} lanzador;

lanzador estado_lanzador;


void cb_lanzador_CABECEO(void) { estado_lanzador = POS_CAB; }
void cb_lanzador_LISTO_PREVIO_CABECEO(void) { estado_lanzador = LISTO_PREVIO_CABECEO; }
void cb_lanzador_LISTO_CABECEO(void) { estado_lanzador = LISTO_CABECEO; }

void cb_lanzador_CARGAR(void) {//////////////////////////////////////////////////////////////////////////////
	estado_lanzador = POS_CARGAR;
	a_cero_lanzador = true;
}

void config(void) {
	
	setupLanzador();
	setup_rebotes();
	setup_carga_retorno(boton_main);
	
	init_bolos();
	init_display();
	
	estado_maq = INICIO;
	estado_juego = STBY;
	
	temp_global = 0;
	temp_partida = 0;
	temp_sincro = 0;
	
	puntuacion = 0;
	
	a_cero_retorno = false;
	

	
	return;
}


void disparar(void) {//////////////////////////////////////////////////////////////////////////////////////////
	if (!ha_disparado) {
		disparo(check_ha_disparado);
		estado_maq = RETORNO;
	}
	
	return;
}


void boton_main(void) {
	
	//ledOn();  // DEBE ENCENDERSE EL LED al pulsar  ////////////////////////////////////////////
	
	if (estado_maq == INICIO) {
		temp_global = 0;
		estado_maq = P_CERO;
		estado_juego = JUEGO; //////////////////////////////////////////////////////////////////////////////
		reiniciar_juego();
	}
	
	if (estado_maq == CABECEO) {
		if (estado_juego == JUEGO || estado_juego == JUEGO_FINAL) {
			estado_maq = DISPARO;
			temp_sincro = 0;
		}
	}
	
	ha_calculado = false;
	
	return;
}

void puesta_a_cero(void) {
	
		//a_cero_retorno = false;
		a_cero_carga   = false;
		
		estado_lanzador = POS_PRE_CERO;
		
		ha_cargado   = false;
		ha_disparado = false;
		ha_subido    = false;
		ha_retornado = false;
	
	if (!a_cero_carga) {
		subir_carga(check_cero_carga);
	}
	if (!a_cero_retorno) {
		bajar_retorno(check_cero_retorno);
	}
	if (estado_lanzador == POS_PRE_CERO) {
		ceroLanzador(cb_lanzador_CARGAR);
	}
	if (a_cero_carga && a_cero_lanzador && (estado_lanzador == POS_CARGAR)) {
		estado_maq = CARGA;
	}
	return;
}


void timer(void) {
	
	// 1ms
	temp_global++;
	temp_sincro++;
	temp_partida++;
	
	timerDisplay();
	
	if (estado_maq == P_CERO) {
		if (temp_global > T_TIMEOUT) {

			// timeout de P_CERO (por basura de f(x))
			// hacer_retorno();
			estado_maq = CARGA;
		}
		
	}
	if (estado_juego == JUEGO) {
		if (temp_partida > T_PARTIDA) {
			puntuacion = 100;
			estado_juego = JUEGO_FINAL;
		}
	}
}



void cargar_bola(void) {
	
	if (!ha_cargado) {
		carga(check_ha_cargado);
		
		// Timeout para la carga
	}
	
	switch(estado_lanzador) {
		case POS_CARGAR:
		cargarDisparo(cb_lanzador_LISTO_PREVIO_CABECEO);
		break;
		
		case LISTO_PREVIO_CABECEO:
		prepararCabeceo(cb_lanzador_LISTO_CABECEO);
		break;
		
		case LISTO_CABECEO:
		if (ha_cargado) {
			estado_maq = CABECEO;
			ha_cargado = false;
		}
		break;
		
		default:
		break;
		
	}
	
	ha_calculado = false;
	
	return;
}

void retornar(void) {
	
	
	if (temp_sincro > T_PISTA) {
		if (temp_sincro < T_RETORNO) {
			retorno_ciclo();
		}
		//retorno_ciclo();
		// Hago el retorno, actualizo la puntuación ...
		
		if (!ha_calculado) {
			int partida = obtener_puntaje_total();
			puntuacion = puntuacion + partida;
			puntuacion_display = puntuacion;
			ha_calculado = true;
			reiniciar_conteo();
		}
		
		if (!ha_subido) {
			
			subir_retorno(check_ha_subido);
			ha_subido = false;
		}
		if (estado_juego == JUEGO_FINAL) {
			estado_maq = INICIO;
			estado_juego = STBY;
		}
		if (estado_juego == JUEGO) {
			estado_maq = P_CERO;
		}
		
	}
	
	
	
	return;
	
}


void iniciar(void) {
	puntuacion = 0;
	puntuacion_display = 0;
	
	return;
}

int main(int argc, const char * argv[]) {
	
	config();
	
	while(1) {
		
		mostrar_numero(puntuacion_display);
		
		switch(estado_maq) {
			
			case INICIO:
			iniciar();
			break;
			
			case P_CERO:
			puesta_a_cero();
			break;
			
			case CARGA:
			cargar_bola();
			break;
			
			case CABECEO:
			cabeceo();
			break;
			
			case DISPARO:
			disparar();
			break;
			
			case RETORNO:
			retornar();
			
			// retorno(); -- hacer_retorno();
			break;
			
			default:
			break;
		}
		
	}
}


