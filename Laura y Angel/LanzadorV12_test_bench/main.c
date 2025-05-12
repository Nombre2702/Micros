/*
 * LanzadorSolo.c
 *
 * Created: 09/05/2025 19:29:04
 * Author : ASUS
 */ 

#include <avr/io.h>
#include "lanzador.h"

typedef enum {
	CERO,
	CARGAR_DISPARO,
	JUEGO,
	DISPARO,
	CARGA,
	LAST_DISPARO
}EstadoLanzador;

typedef enum {
	LISTO_CABECEO,
	NO_LISTO_CABECEO
}EstadoCabeceo;

typedef enum {
	IDLE_DISPARO,
	LISTO_DISPARO,
	NO_LISTO_DISPARO	
}EstadoDisparo;

volatile EstadoLanzador estado_actual_lanzador;
volatile EstadoCabeceo estado_actual_cabeceo;
volatile EstadoDisparo estado_actual_disparo;

volatile uint8_t init_delay_carga = 0;
volatile uint16_t delay_carga = 0;

volatile uint8_t init_delay_disparo = 0;
volatile uint16_t delay_disparo = 0;

void delayMain(){
	if(init_delay_carga){ delay_carga++; }
	
	if(init_delay_disparo){ delay_disparo++; }
}

void setDisparo(){
	estado_actual_lanzador = DISPARO;
}

void setCarga(){
	estado_actual_lanzador = CARGA;
	estado_actual_disparo = NO_LISTO_DISPARO;
}

void onCeroCompleto(){
	estado_actual_lanzador = CARGAR_DISPARO;
}

void onCabeceoPreparado(){
	estado_actual_cabeceo = LISTO_CABECEO;
}

void onDisparoListo(){
	estado_actual_disparo = LISTO_DISPARO;
	estado_actual_lanzador = JUEGO;
}

void onDisparoRealizado(){
	estado_actual_disparo = IDLE_DISPARO;
	estado_actual_cabeceo = NO_LISTO_CABECEO;
}

void onCargaPreparada(){
	estado_actual_lanzador = LAST_DISPARO;
	estado_actual_cabeceo = NO_LISTO_CABECEO;
}

int main(void)
{
	setupLanzador();
	
	estado_actual_lanzador = CERO;
	estado_actual_cabeceo = NO_LISTO_CABECEO;
	estado_actual_disparo = NO_LISTO_DISPARO;
	/*
    while (1) 
    {
		ceroLanzador();
    }
	*/
	while(1){
		switch(estado_actual_lanzador){
			case CERO:
				//flancos_detectados = 0xFF;					// para simular
				ceroLanzador(onCeroCompleto);				// CERO --> CARGAR DISPARO
			break;
			case CARGAR_DISPARO:
				//flancos_detectados = 0xFF;					// para simular
				cargarDisparo(onDisparoListo);				// NO_LISTO_DISPARO --> LISTO_DISPARO & CARGAR_DISPARO --> JUEGO
			break;
			case JUEGO:
				//flancos_detectados = 0xFF;					// para simular
				if(estado_actual_cabeceo == NO_LISTO_CABECEO){
					prepararCabeceo(onCabeceoPreparado);	// NO_LISTO_CABECEO --> LISTO_CABECEO
					init_delay_disparo = 1;
				}
				//flancos_detectados = 0xFF;
				if(estado_actual_cabeceo == LISTO_CABECEO){
					cabeceo();
					//delay_disparo = 10000;					// para simular
					if (delay_disparo >= 10000){
						setDisparo();						// JUEGO --> DISPARO
						init_delay_disparo = 0;
						delay_disparo = 0;
					}
				}
			break;
			case DISPARO:
				//flancos_detectados = 0xFF;
				if (estado_actual_disparo == LISTO_DISPARO){
					disparo(onDisparoRealizado);			// LISTO_DISPARO --> IDLE_DISPARO & LISTO_CABECEO --> NO_LISTO_CABECEO
					init_delay_carga = 1;
				}
				//delay_carga = 10000;						// para simular
				if (delay_carga >= 10000){
					setCarga();								// DISPARO --> CARGA & LISTO_DISPARO --> NO_LISTO_DISPARO
					init_delay_carga = 0;
					delay_carga = 0;
				}
			break;
			case CARGA:
				//flancos_detectados = 0xFF;					// para simular
				prepararCarga(onCargaPreparada);			// CARGA --> LAST_DISPARO
			break;
			case LAST_DISPARO:
				ledParpadeoOn();
			break;
		}
	}
	
}
