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
typedef enum {
	RETORNO_UNKNOWN,
	RETORNO_ARRIBA,
	RETORNO_ABAJO
} retornoStatus;



// MARK: Global vars
int timer_global;
int timer_sync;

int pin_count;
int pin_count_display;

gameState game_state;
machineState machine_state;

lanzadorState lanzador_state;

cargaStatus carga_state;
retornoStatus retorno_state;

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

	// Mostrar estado de retorno
	if (retorno_state == RETORNO_UNKNOWN) {
		printf("| retorno RETORNO_UNKNOWN\n");
		} else if (retorno_state == RETORNO_ARRIBA) {
		printf("| retorno RETORNO_ARRIBA\n");
		} else if (retorno_state == RETORNO_ABAJO) {
		printf("| retorno RETORNO_ABAJO\n");
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

void setRetorno_ARRIBA(void) {
	retorno_state = RETORNO_ARRIBA;
}
void setRetorno_ABAJO(void) {
	retorno_state = RETORNO_ABAJO;
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
	retorno_state = RETORNO_UNKNOWN;
	lanzador_state = POS_UNKNOWN;
	
	showStates();
}

void setCero(void) {
	/*
	Pongo retorno ABAJO, carga ARRIBA, lanzador POS_CARGA;
	*/
	
	
	bajar_retorno(setRetorno_ABAJO);
	subir_carga(setCarga_ARRIBA);
	if (carga_state == CARGA_ARRIBA) {
		ceroLanzador(setLanzador_CARGA);
		if ((lanzador_state == POS_CARGA) && (retorno_state == RETORNO_ABAJO)) {
			setMachine_LOADING();
		}
		
	}
	
	return;
}


void reload(void) {
	
	
	// Lógica de carga ... 
	
	
}

int main(void)
{

	
    /* Replace with your application code */
	setup();
    while (1) 
    {
		
		
		switch(machine_state) {
			
			
			
			case SET_CERO:
				setCero();
			break;
			
			case LOADING:
			
			break;
			
			case READY: // Cargado; listo para disparar
				switch(game_state) {
					
					case STANDBY:
					break;
					
					case GAME:
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
		//barreraArriba();
		//ceroLanzador();
		//ledParpadeoOn();
		//cabeceo();
		//prepararCarga();
		//cargarDisparo();
		//disparo();
    }
}

