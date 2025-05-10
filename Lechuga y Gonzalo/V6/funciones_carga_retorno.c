#include "carga_retorno.h"
#include "lanzador.h"
#include "integracion.h"

// VARIABLES DEBOUNCER LANZADOR
volatile uint8_t estado_anterior = 0xFF;
volatile uint8_t estado_actual = 0;

//INICIALIZAR VARIABLES INTERNAS DE ESTADO
volatile uint8_t motor_carga = 0;	//Hacia donde va la carga
volatile uint8_t motor_retorno = 0;	//Hacia donde va el retorno
volatile uint8_t ret_arriba = 2;	//Donde esta el retorno (limites)
volatile uint8_t car_arriba = 2;	//Donde esta la carga (limites)
volatile uint8_t desactivar_retorno=0;	//Señal de deshabilitacion del retorno
volatile uint8_t desactivar_carga=0;	//Señal de deshabilitacion de la carga
volatile uint8_t hab_car_int =1;	//Habilitacion interna de la carga
volatile uint8_t hab_ret_int =1;	//Habilitacion interna del retorno
volatile uint8_t boton = 0;		//Estado del boton

void setup_rebotes(){
	cli();
	TCCR3A &= ~(1 << COM3A0);	//Modo de funcionamiento normal
	TCCR3A &= ~(1 << COM3A1);	//Modo de funcionamiento normal
	TCCR3B |= (1 << CS31);		//Preescalado de 8
	TCCR3B &= ~(1 << CS30);		//Preescalado de 8
	TCCR3B &= ~(1 << CS32);		//Preescalado de 8
	TCNT3 = 0x3CAF;			//Inica la cuenta para que solo queden 50000, 50ms, evita la necesidad de output compares
	TIMSK3 |= (1 << TOIE3);		//Habilita interrupciones
	sei();
}

void setup_carga_retorno(){
	cli();
	
	//FAST PWM CON LIMITE EN ICR1 Y OCRA OCRB OCRC
	TCCR1A &= ~(1 << WGM10);
	TCCR1A |= (1 << WGM11);
	TCCR1B |= ((1 << WGM12) | (1 << WGM13));
	ICR1= 0xFFFF;	//Max en ICR1 al maximo
	
	//PREESCALADO DE 8
	TCCR1B &= ~((1 << CS10) | (1 << CS12));
	TCCR1B |= (1 << CS41);
	
	//OUPUT COMPARES
	TCCR1A |= (1 << COM1A1);	//non inverting
	TCCR1A &= ~(1 << COM4A0);	//non inverting
	TCCR1A |= (1 << COM1B1);	//non inverting
	TCCR1A &= ~(1 << COM1B0);	//non inverting
	OCR1A = 0.25*ICR1;		//Duty cicle de 25% para primer PWM
	OCR1B = 0.25*ICR1;		//Duty cicle de 25% para segundo PWM
	
	//SALIDAS DEL PWM
	DDRB |= ((1 << PB5) | (1 << PB6));	//PB5 y PB6 como salidas
	
	//ENTRADAS DE LOS FINES DE CARRERA
	DDRB &= ~(1 << PB0);	//PB0 como entrada
	DDRB &= ~(1 << PB1);	//PB1 como entrada 
	DDRB &= ~(1 << PB7);	//PB7 como entrada
	PORTB |= (1 << PB0);	//Resistencia Pull-up de PB0
	PORTB |= (1 << PB1);	//Resistencia Pull-ip de PB1
	PORTB |= (1 << PB7);	//Resistencia Pull-ip de PB7

	//SALIDAS PARA ELEGIR EL DIR
	DDRK |= ((1 << PK6) | (1 << PK7));
	
	//HABILITAR INTERRUPCIONES
	PCICR |= (1 <<PCIE0);				//Habilitar interrupciones de PCINT del 0 al 7
	PCMSK0 |= ((1 << PCINT0)|(1 << PCINT1));	//Habilitar interrupciones especificas
	sei();
}

ISR(TIMER3_OVF_vect){
	TCNT3=0x3CAF;				//Devuelve la cuenta a donde le toca
	if((habilita>0)&&(habilita<5)){		//Requiere 4 ciclos de 50 ms para rehabilitar las interrupciones
		habilita++;
	}
	if(habilita==5){					//Comprueba si tiene permiso a rehabilitar las interrupciones, tras 4 ciclos de 50 ms, 200 ms
		PCMSK0 |= ((1 << PCINT0) | (1 << PCINT1));	//Habilita las interrupciones especificas
		habilita = 0;					//Cancela la habilitación	
	}
}

ISR(PCINT0_vect){
	estado_actual = PINB;

	antirrebotesLanzador(SW2);
	antirrebotesLanzador(SW3);
	antirrebotesLanzador(SW4);
	
	estado_anterior = estado_actual;
	
	if(!(PINB & (1 << PB0))){	//Comprueba que el cambio del pin haya B0 sido flanco de bajada
		finalRetorno();
	}

	if(!(PINB & (1 << PB1))){	//Comprueba que el cambio del pin B1 haya sido flanco de bajada
		finalCarga();
	}
	if(!(PINB & (1 << PB7))){
		botonPulsado();
	}
}

void finalCarga(){
	PCMSK0 &= ~(1 << PCINT1);	//Deshabilita la interrupcion
	habilita = 1;			//Permite que se pueda rehabilitar la interrupcion
	TCNT3=0x3CAF;			//Pone la cuenta a falta de 50000, para asegurarnos de que pasan 50 ms
	TCCR1B &= ~(1 << COM1B1);	//Desactiva salida del PWM
	TCCR1B &= ~(1 << COM1B0);	//Desactiva salida del PWM
	if (motor_carga==2){		//Comprueba si estaba bajando
		motor_carga=1;		//Avisa de que el motor pasa a estar encendido subiendo
		car_arriba = 0;		//Avisa de que esta abajo
	}
	if (motor_carga==1){		//Comprueba si estaba subiendo
		motor_carga=0;		//Avisa de que el motor pas a estar parado
		car_arriba = 1;		 //Avisa de que esta arriba
	}
}

void finalRetorno(){
        PCMSK0 &= ~(1 << PCINT0);	//Deshabilita la interrupcion
	habilita = 1;			//Permite que se pueda rehabilitar la interrupcion
	TCNT3=0x3CAF;			//Pone la cuenta a falta de 50000, para asegurarnos de que pasan 50 ms
	TCCR1A &= ~(1 << COM1A1);	//Desactiva salida del PWM
	TCCR1A &= ~(1 << COM1A0);	//Desactiva salida del PWM
	if (motor_retorno==1){		//Comprueba si estaba subiendo
		motor_retorno=2;	//Avisa de que el motor pasa a estar encendido bajando
		ret_arriba = 1;		//Avisa de que esta arriba
		
	}
	if (motor_retorno==2){		//Comprueba si estaba bajando
		motor_retorno=0;	//Avisa de que el motor pas a estar parado
		ret_arriba = 0;		//Avisa de que esta abajo
	}
}

int getBoton() {
	return boton;
}

int leer_motor_retorno(){
	return motor_retorno;	//Devuelve el estado del motor de retorno
}

int leer_motor_carga(){
	return motor_carga;	//Devuelve el estado del motor de carga
}

void subir_retorno(void (*callback)(void)) {
	PORTK |= (1 << PK6);		//Dir M5 es K6, lo pone a 1 para subir
	TCCR1A |= (1 << COM1A1);	//Activa la salida del PWM
	TCCR1A &= ~(1 << COM1A0);	//Activa la salida del PWM
	motor_retorno=1;		//Avisa de que el motor esta encendido
	if(ret_arriba == 1){
		callback();
	}
}

void bajar_retorno(void (*callback)(void)){
	PORTK &= ~(1 << PK6); 		//Dir M5 es K6, lo pone a 0 para bajar
	TCCR1A |= (1 << COM1A1);	//Activa la salida del PWM
	TCCR1A &= ~(1 << COM1A0);	//Activa la salida del PWM
	motor_retorno=2;		//Avisa de que el motor esta encendido
	if(ret_arriba == 0){
		callback();
	}
}

void subir_carga(void (*callback)(void)){
	PORTK |= (1 << PK7);		 //Dir M1 es K7, lo pone a 0 para subir
	TCCR1B |= (1 << COM1B1);	//Activa la salida del PWM
	TCCR1B &= ~(1 << COM1B0);	//Activa la salida del PWM
	motor_carga=1;			//Avisa de que el motor esta encendido
	if(car_arriba == 1){
		callback();
	}
}

void bajar_carga(void (*callback)(void)){
	PORTK &= ~(1 << PK7);		 //Dir M1 es K7, lo pone a 0 para bajar
	TCCR1B |= (1 << COM1B1);	//Activa la salida del PWM
	TCCR1B &= ~(1 << COM1B0);	//Activa la salida del PWM
	motor_carga=2;			//Avisa de que el motor esta encendido
	if(car_arriba == 0){
		callback();
	}
}

void carga(void(*callback)(void)){
	desactivar_retorno=0;				//Reactiva el retorno
	if (desactivar_carga==0){			//Si esta habilitado, manda motores hacia abajo
		if (hab_car_int==1){			//Comprueba si esta habilitado
			PORTK &= ~(1 << PK7);		//Dir M1 es K7, lo pone a 0 para bajar
		        TCCR1B |= (1 << COM1B1);	//Activa la salida del PWM
		        TCCR1B &= ~(1 << COM1B0);	//Activa la salida del PWM
			motor_carga=2;			//Avisa de que esta bajando
			hab_car_int=0;			//Desactiva, para que solo ponga los motores hacia abajo una vez
		}
		if (car_arriba==0){			//Comprueba si la carga ha llegado abajo
			PORTK |= (1 << PK7);		//Dir M1 es K7, lo pone a 1 para subir
			TCCR1B |= (1 << COM1B1);	//Activa la salida del PWM
			TCCR1B &= ~(1 << COM1B0);	//Activa la salida del PWM
			motor_carga=1;			//Avisa de que el motor esta encendido hacia arriba
			desactivar_carga= 1;		//Como ya ha puesto hacia arriba y completado el ciclo, desactiva
			hab_car_int=1;			//Reactiva que se puedan poner los motores hacia abajo de nuevo
		}
	}
	if((desactivar_carga==1)&&(car_arriba==1){ //Si ha llegado arriba y ha completado el ciclo, avisa de que ha acabado
		callback();
	}
}

void retorno(){
	desactivar_carga=0;				//Reactiva la carga
	if(desactivar_retorno == 0){			//Si esta habilitado, manda motores hacia arriba
		if (hab_ret_int==1) {			//Comprueba que este habilitado
	                PORTK |= (1 << PK6);		//Dir M5 es K6, lo pone a 1 para subir
		        TCCR1A |= (1 << COM1A1);	//Activa la salida del PWM
		        TCCR1A &= ~(1 << COM1A0);	//Activa la salida del PWM
		        motor_retorno=1;		//Avisa de que el motor esta encendido hacvia arriba
			hab_ret_int=0;			//Deshabilita, para solo ponerlo hacia arriba una vez
		}
		if (ret_arriba==1){			//Comprueba si ha llegado arriba
			PORTK &= ~(1 << PK6);		//Dir M5 es K6, lo pone a 0 para bajar
			TCCR1A |= (1 << COM1A1);	//Activa la salida del PWM
			TCCR1A &= ~(1 << COM1A0);	//Activa la salida del PWM
			motor_retorno=2;		//Avisa de que el motor esta encendido bajando
			desactivar_retorno = 1;		//Desactiva el retorno, al haber completado ya el ciclo
			hab_ret_int=1;			//Reactiva que se puedan poner los motores hacia arriba de nuevo
		}
	}
}

void ceroRetorno(void(*callback)()){
	bajar_retorno();	//Baja el retorno
	subir_carga();		//Sube la carga
	callback();
}


