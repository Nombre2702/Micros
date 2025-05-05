#include <avr/io.h>
#include <avr/interrupt.h>
#include "funciones.h"
#include "funciones_elementales.h"


void inicio(){
	//    contador_30(); //esto puedo hacerlo así para que lo gestionen los de integración? o tengo que hacerlo yo
	barrera_arriba();
	rotarDerecha();
	moverAdelante();
}

void cargar_bola(){
	barrera_arriba();
	ceroPlataforma(contador);
	cargar_tiro();
}

void cabeceo(){
	flag_b=0;
	rotarIzquierda();
	//aqui yo pondría un timer para poder pasar del sensor del medio pero no sé muy bien como
	//y de aquí lo hace la interrupción
}

void cargar_tiro(){

	barrera_abajo();
	cabeceo(); // puedo cambiar un poco el funcionamiento y hacer que el vástago se empiece
	// en el medio que yo creo que es más fácil
	moverAtras(); // en principio esto se debería hacerse a la vez que lo del cabeceo
	// que lo hace la interrupción

	flag_b=1;
}



void disparo(int contador_final_carrera_3){
	// el duty que le pases será la fuerza con la que tire
	// se puede meter algún tipo de seguridad interna en la función
	// no la pongo para que les sea más fácil a los de integración

	// init
	
		pararPlataforma();
		barrera_arriba();
		moverAdelante(); //supongo que esto habrá que ponerlo al máximo
		//volver al inicio
		ceroPlataforma(0);
		// lógica
		// lanza hasta llegar al final que para

		//yo creo que esto no hace falta porque disparo solo tiene que subir la barrera y
		//empujar el vástago
		if(contador_final_carrera_3 == 1){
			moverAdelante();
		}
		else if(contador_final_carrera_3 == 2){
			pararVastago();
		}

		flag_b=0;
	
}


void balanceoLanzador(int contador_final_carrera_2, int sentido_giro){
	// rota, inicialmente, hacia el sentido que nos diga el programa principal
	// cuando llega a un extremo cambia de sentido
    PORTL |= (1 << PL6);
    rotarDerecha();
    flag = 0;
	//yo dejaría que esto lo hiciese la interrupción
	if (contador_final_carrera_2 == 3 && sentido_giro == 0){
		pararPlataforma();
		rotarDerecha();
		contador_final_carrera_2 = 0;
		sentido_giro != sentido_giro;
	}
	else if(contador_final_carrera_2 == 3 && sentido_giro == 1){
		pararPlataforma();
		rotarIzquierda();
		contador_final_carrera_2 = 0;
		sentido_giro != sentido_giro;
	}
	else if(contador_final_carrera_2 < 3 && sentido_giro == 0){
		rotarIzquierda();
	}
	else if(contador_final_carrera_2 < 3 && sentido_giro == 1){
		rotarDerecha();
	}
}


// desactivar interrupcón para antirrebotes
//