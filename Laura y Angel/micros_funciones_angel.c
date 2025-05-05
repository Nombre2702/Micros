	void setupLanzador(){
	// FINALES DE CARRERA	
		// config entradas switches | puerto K
		DDRK &= ~((1 << PK2) | (1 << PK1) | (1 << PK0));
		
	// DIRECCIONES
		// config salidas direcciones | puerto L
		DDRL |= (1 << PL2) | (1 << PL1) | (1 << PL0);
		
	// ENABLES
		// setup del timer 5 | puerto L | Fast PWM
		// config salidas puerto L con enmascaramiento
		DDRL |= (1 << PL5) | (1 << PL4) | (1 << PL3);
		
		// config Fast PWM con ICR5 (1 1 1 0)
		TCCR5A &= ~(1 << WGM50);
		TCCR5A |= (1 << WGM51);
		TCCR5B |= (1 << WGM52);
		TCCR5B |= (1 << WGM53);
		
		// config prescaler (8)
		TCCR5B |= (1 << CS51);
		TCCR5B &= ~((1 << CS50) | (1 << CS52));
		
		// config output compares non-inverted (1 0)
		OCR5A = duty
		TCCR5A |= (1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
		TCCR5A |= (1 << COM5C1); TCCR5A &= ~(1 << COM5C0);
		
		// config del TOP | ICR5 | es un registro de 16 bits asi que para aprovecharlo al maximo lo ponemos a 0xFFFF
		ICR5 = 0xFFFF;
	}

	// FUNCIONES PLATAFORMA | motor 2 | sw 2

	void pararPlataforma(){
		TCCR5A &= ~(1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
	}

	void rotarIzquierda(){
		// configurar dirección
		PORTL |= (1 << PL0);
		// duty nuevo: se configura con el COM
		TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
	}

	void rotarDerecha(){
		// configurar dirección
		PORTL &= ~(1 << PL0);
		// duty nuevo
		TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
	}

	void ceroPlataforma(int contador){
		// se puede hacer algo con los finales de carrera
		// habría que medir tiempos antes
		if (contador == 2){
			pararPlataforma();
			contador = 0;
		}else{
			rotarDerecha();
		}
	}

	void balanceoLanzador(int contador_final_carrera_2, int sentido_giro){
		// rota, inicialmente, hacia el sentido que nos diga el programa principal
		// cuando llega a un extremo cambia de sentido
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

	// FUNCIONES VÁSTAGO

	void moverAdelante(){
		// configurar dirección
		PORTL |= (1 << PL1);
		// duty nuevo
		TCCR5A |= (1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
	}

	void moverAtras(){
		// configurar dirección
		PORTL &= ~(1 << PL1);
		// duty nuevo
		TCCR5A |= (1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
	}

	void pararVastago(){
		TCCR5A &= ~(1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
	}

	void ceroVastago(int contador){
		// se mueve hasta llegar alante del todo
		// falta determinar el tiempo y la escala temporal
		if(contador == 2){
			pararVastago();
			contador = 0;
		}else{
			moverAdelante();
		}
	}
	*
	void disparo(int contador_final_carrera_3){
		// el duty que le pases será la fuerza con la que tire
		// se puede meter algún tipo de seguridad interna en la función
		// no la pongo para que les sea más fácil a los de integración
		
		// init
		pararPlataforma();
		pararVastago();
		
		// lógica
		// lanza hasta llegar al final que para
		if(contador_final_carrera_3 == 1){
			moverAdelante();
		}
		else if(contador_final_carrera_3 == 2){
			pararVastago();
			contador_final_carrera_3 = 0;
		}
	}