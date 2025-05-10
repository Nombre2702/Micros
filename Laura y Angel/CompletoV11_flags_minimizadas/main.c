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
	JUEGO,
	CARGA,
	LAST_DISPARO
}EstadoLanzador;

typedef enum {
	LISTO_CABECEO,
	NO_LISTO_CABECEO
}EstadoCabeceo;

typedef enum {
	LISTO_DISPARO,
	NO_LISTO_DISPARO	
}EstadoDisparo;

volatile EstadoLanzador estado_actual_lanzador;
volatile EstadoCabeceo estado_actual_cabeceo;
volatile EstadoDisparo estado_actual_disparo;

void onCeroCompleto(){
	estado_actual_lanzador = JUEGO;
}

void onCabeceoPreparado(){
	estado_actual_cabeceo = LISTO_CABECEO;
}

void onDisparoListo(){
	estado_actual_disparo = LISTO_DISPARO;
}

void onDisparoRealizado(){
	estado_actual_disparo = NO_LISTO_DISPARO;
	estado_actual_cabeceo = NO_LISTO_CABECEO;
	estado_actual_lanzador = CARGA;
}

void onCargaPreparada(){
	estado_actual_lanzador = LAST_DISPARO;
}

int main(void)
{
	setupLanzador();
	
	estado_actual_lanzador = CERO;
	estado_actual_cabeceo = NO_LISTO_CABECEO;
	estado_actual_disparo = NO_LISTO_DISPARO;
	
    while (1) 
    {
		switch(estado_actual_lanzador){
			case CERO:
				ceroLanzador(onCeroCompleto);				// cuando se complete la pupesta a cero --> lanzador = JUEGO
				break;
			case JUEGO:
				if(estado_actual_cabeceo == NO_LISTO_CABECEO){
					prepararCabeceo(onCabeceoPreparado);	// cuando esté preparado el cabeceo --> cabeceo = LISTO
				}
				if(estado_actual_cabeceo == LISTO_CABECEO){
					cargarDisparo(onDisparoListo);			// cuando cargue --> disparo = LISTO
					cabeceo();
					if(estado_actual_disparo == LISTO_DISPARO){
						_delay_ms(2000);
						disparo(onDisparoRealizado);		// cuando se dispare --> lanzador = CARGA, cabeceo = NO_LISTO, disparo = NO_LISTO
						_delay_ms(2000);
					}
				}
				break;
			case CARGA:
				prepararCarga(onCargaPreparada);			// cuando esté preprada la carga --> lanzador = LAST_DISPARO
				break;
			case LAST_DISPARO:
				ledParpadeoOn();							// FIN DE LA DEMOSTRACIÓN
				break;	
		}
    }
}

