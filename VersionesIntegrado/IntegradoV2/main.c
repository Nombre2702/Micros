/*
 * CompletoV8.c
 *
 * Created: 08/05/2025 17:55:28
 * Author : ASUS
 */ 

#include <avr/io.h>
#include <stdio.h>

#include "lanzador.h"
#include "carga_retorno.h"
#include "integracion.h"

#define T_PARTIDA 30000
#define T_PISTA 2500
#define T_RAMPA 1000




typedef enum {
	GAME,
	GAME_FINAL,
	STANDBY
} gameState;
typedef enum {
	SET_CERO,
	LOADING,
	READY
} machineState;
typedef enum {
	POS_UNKNOWN,
	POS_CARGA,
	POS_CABECEO
} lanzadorState;
typedef enum {
	CARGA_UNKNOWN,
	CARGA_ARRIBA,
	CARGA_ABAJO
} cargaStatus;



// MARK: Global vars
int timer_global;
int timer_sync;

int pin_count;
int pin_count_display;

gameState game_state;
machineState machine_state;

lanzadorState lanzador_state;

cargaStatus carga_state;

void showStates(void) {
	// Mostrar estado de la máquina
	if (machine_state == SET_CERO) {
		printf("| machine SET_CERO\n");
		} else if (machine_state == LOADING) {
		printf("| machine LOADING\n");
		} else if (machine_state == READY) {
		printf("| machine READY\n");
	}

	// Mostrar estado del juego
	if (game_state == STANDBY) {
		printf("| game STANDBY\n");
		} else if (game_state == GAME) {
		printf("| game GAME\n");
		} else if (game_state == GAME_FINAL) {
		printf("| game GAME_FINAL\n");
	}

	// Mostrar estado del lanzador
	if (lanzador_state == POS_UNKNOWN) {
		printf("| lanzador POS_UNKNOWN\n");
		} else if (lanzador_state == POS_CARGA) {
		printf("| lanzador POS_CARGA\n");
		} else if (lanzador_state == POS_CABECEO) {
		printf("| lanzador POS_CABECEO\n");
	}

	// Mostrar estado de carga
	if (carga_state == CARGA_UNKNOWN) {
		printf("| carga CARGA_UNKNOWN\n");
		} else if (carga_state == CARGA_ARRIBA) {
		printf("| carga CARGA_ARRIBA\n");
		} else if (carga_state == CARGA_ABAJO) {
		printf("| carga CARGA_ABAJO\n");
	}

	
}

void setMachine_SET_CERO(void) {
	machine_state = SET_CERO;
}
void setMachine_LOADING(void) {
	machine_state = LOADING;
}
void setMachine_READY(void) {
	machine_state = READY;
}

void setGame_GAME(void) {
	game_state = GAME;
}
void setGame_GAME_FINAL(void) {
	game_state = GAME_FINAL;
}
void setGame_STANDBY(void) {
	game_state = STANDBY;
}


void setCarga_ARRIBA(void) {
	carga_state = CARGA_ARRIBA;
}
void setCarga_ABAJO(void) {
	carga_state = CARGA_ABAJO;
}



void setLanzador_CARGA(void) {
	lanzador_state = POS_CARGA;
}
void setLanzador_CABECEO(void) {
	lanzador_state = POS_CABECEO;
}


void setup(void) {
	/* Cfg de pines */
	setupLanzador();
	setup_carga_retorno();
	setup_rebotes();
	
	/* Cfg estado_i de la máquina:
	
	*/
	machine_state = SET_CERO;
	game_state = STANDBY;
	
	carga_state = CARGA_UNKNOWN;
	lanzador_state = POS_UNKNOWN;
	
	showStates();
}

void setCero(void) {
	/*
	Pongo retorno ABAJO, carga ARRIBA, lanzador POS_CARGA;
	Ahora el retorno no tiene estados, solo funciona con elevador()
	*/
	
	
	bajar_retorno(); // El elevaador siempre va a estar abajo
	
	subir_carga(setCarga_ARRIBA);
	if (carga_state == CARGA_ARRIBA) {
		ceroLanzador(setLanzador_CARGA);
		if (lanzador_state == POS_CARGA) {
			setMachine_LOADING();
		}
		
	}
	
	return;
}


void (*cargar_callback_final)(void) = NULL;

void alSubir(void) {
	carga_state = CARGA_ARRIBA;
	if (cargar_callback_final != NULL) {
		cargar_callback_final();
		cargar_callback_final = NULL; // Limpiar después de usar
	}
}

void alBajar(void) {
	carga_state = CARGA_ABAJO;
	subir_carga(alSubir); // Subir después de bajar
}

void cargar(void (*callback)(void)) {
	if (carga_state == CARGA_ARRIBA) {
		cargar_callback_final = callback; // Guardar el callback
		bajar_carga(alBajar);            // Inicia el ciclo bajando
	}
}



void reload(void) {
	
	
	
	cargar(setCarga_ARRIBA);
	
	if (carga_state == CARGA_ARRIBA) {
		
		if (lanzador_state == POS_CARGA) {
			prepararCabeceo(setLanzador_CABECEO);
		}
		if (lanzador_state == POS_CABECEO) {
			cargarDisparo(setMachine_READY);
		}
	}
}

int terminado;
void setTerminado() {
	terminado = 1;
}
void fire(void) {
	int terminado = 0;
	
	disparo(setTerminado);
	if (terminado == 1) {
		prepararCarga(setMachine_LOADING);
	}
}



void botonPulsado() {
	if (machine_state == READY) {
		switch(game_state) {
			
			case STANDBY:
			// No hago nada, estoy cargado pero al final de la partida o justo tras el SET_CERO
			break;
			
			
			case GAME:
			// Disparo
			fire();
			break;
			
			case GAME_FINAL:
			// Dispara y acaba la partida
			fire();
			setGame_STANDBY();
			break;
			
			default:
			break;
			
		}
		
	}
	
	
}


void timer(void) {
	
	
	
}
int main(void)
{

	
    /* Replace with your application code */
	setup();
    while (1) 
    {

		if (getBoton() == 1) {
			botonPulsado();
			boton = 0;
		}
		
		switch(machine_state) {
			
			
			
			case SET_CERO:
				setCero();
			break;
			
			case LOADING:
			reload();
			
			break;
			
			case READY: // Cargado; listo para disparar
				switch(game_state) {
					
					case STANDBY:
					break;
					
					case GAME:
						cabeceo();
					break;
					
					case GAME_FINAL:
					break;
					
					default:
					break;
				}
			break;
			
			
			default:
			break;
		}
    }
}

