/*
 * funciones_integracion.c
 *
 * Created: 06/05/2025 23:41:48
 *  Author: ASUS
 *
 *	Aquí se incluyen las funciones que usará el equipo de integración
 */ 

#include "lanzador.h"

// PREPARACION CABECEO 
	// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)

// CABECEO
	// sabemos que partimos del medio
	// movemos a la izda hasta que el antirrebotes detecte SW2 una vez (flanco bajada)
	// cuando lo detecte cambiamos de direccion (se me ocurre con hacerlo con una variable direccion y un if)
	
// CARGA LANZADOR
	// se va hasta la izquierda del todo, sabemos que vamos a estar en algún punto intermedio así que solo trenemos que esperar a un flanco de bajada de SW2
	// luego se mueve hacia la derecha hasta que se detectan 2 flancos de bajada, indicando que ya está en el final

// CARGA DISPARO

// DISPARO