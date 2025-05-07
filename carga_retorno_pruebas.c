/*
 * main.c
 *
 * Created: 5/2/2025 11:52:42 AM
 *  Author: jlech
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

//INICIALIZAR VARIABLES INTERNAS DE ESTADO
volatile uint8_t motor_carga = 0;
volatile uint8_t motor_retorno = 0;
volatile uint8_t motor_carga_status = 0;
volatile uint8_t motor_retorno_status = 0;
volatile uint8_t habilita = 0;

void setup_rebotes(){
	cli();
	TCCR3B &= ~(1 << COM3A0);	//Modo de funcionamiento normal
	TCCR3B &= ~(1 << COM3A1);	//Modo de funcionamiento normal
	TCCR3B |= (1 << CS31);	//Preescalado de 8
	TCCR3B &= ~(1 << CS30);	//Preescalado de 8
	TCCR3B &= ~(1 << CS32);	//Preescalado de 8
	TCNT3 = 0xD8F0;	//Inica la cuenta para que solo queden 10000, 10ms, evita la necesidad de output compares
	TIMSK3 |= (1 << TOIE3);	//Habilita interrupciones
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
	OCR1A = 0.25*ICR1;	//Duty cicle de 25% para primer PWM
	OCR1B = 0.25*ICR1;	//Duty cicle de 25% para segundo PWM
	
	//SALIDAS DEL PWM
	DDRB |= ((1 << PB5) | (1 << PB6));	//PB5 y PB6 como salidas
	
	//ENTRADAS DE LOS FINES DE CARRERA
	DDRB &= ~(1 << PB0);	//PB0 como entrada
	DDRB &= ~(1 << PB1);	//PB1 como entrada 
	PORTB |= (1 << PB0);	//Resistencia Pull-up de PB0
	PORTB |= (1 << PB1);	//Resistencia Pull-ip de PB1
	
	//HABILITAR INTERRUPCIONES
	PCICR |= (1 <<PCIE0);	//Habilitar interrupciones de PCINT del 0 al 7
	PCMSK0 |= ((1 << PCINT0) | (1 << PCINT1));	//Habilitar interrupciones especificas
	sei();
}

ISR(TIMER3_OVF_vect){
	TCNT3=0xD8F0;	//Devuelve la cuenta a donde le toca
	if(habilita==1){	//Comprueba si tiene permiso a rehabilitar las interrupciones
		PCMSK0 |= ((1 << PCINT0) | (1 << PCINT1));	//Habilita las interrupciones especificas
		habilita = 0;	//Cancela la habilitación	
	}
}

ISR(PCINT0_vect){
	if(!(PINB & (1 << PB0))){	//Comprueba que el cambio del pin haya sido flanco de bajada
		TCCR1A &= ~(1 << COM1A1);	//Desactiva salida del PWM
		TCCR1A &= ~(1 << COM1A0);	//Desactiva salida del PWM
		motor_retorno=0;	//Avisa de que el motor esta parado
		PCMSK0 &= ~(1 << PCINT0);	//Deshabilita la interrupcion
		habilita = 1;	//Permite que se pueda rehabilitar la interrupcion
		TCNT3=0xD8F0;	//Pone la cuenta a falta de 10000, para asegurarnos de que pasan 10 ms
	}
}

ISR(PCINT1_vect){
	if(!(PINB & (1 << PB1))){	//Comprueba que el cambio del pin haya sido flanco de bajada
		TCCR1B &= ~(1 << COM1B1);	//Desactiva salida del PWM
		TCCR1B &= ~(1 << COM1B0);	//Desactiva salida del PWM
		motor_carga=0;	//Avisa de que el motor esta parado
		PCMSK0 &= ~(1 << PCINT1);	//Deshabilita la interrupcion
		habilita = 1;	//Permite que se pueda rehabilitar la interrupcion
		TCNT3=0xD8F0;	//Pone la cuenta a falta de 10000, para asegurarnos de que pasan 10 ms
	}
}

int leer_motor_retorno(){
	return motor_retorno;	//Devuelve el estado del motor de retorno
}

int leer_motor_carga(){
	return motor_carga;	//Devuelve el estado del motor de carga
}

int boton(){
	return boton;	//Devuelve el estado del boton
}

void subir_retorno(){
	PORTK |= (1 << PK6);	//Dir M5 es K6, lo pone a 1 para subir
	TCCR1A |= (1 << COM1A1);	//Activa la salida del PWM
	TCCR1A &= ~(1 << COM1A0);	//Activa la salida del PWM
	motor_retorno=1;	//Avisa de que el motor esta encendido
}

void bajar_retorno(){
	PORTK &= ~(1 << PK6); //Dir M5 es K6, lo pone a 0 para bajar
	TCCR1A |= (1 << COM1A1);	//Activa la salida del PWM
	TCCR1A &= ~(1 << COM1A0);	//Activa la salida del PWM
	motor_retorno=1;	//Avisa de que el motor esta encendido
}

void subir_carga(){
	PORTK |= (1 << PK7); //Dir M1 es K7, lo pone a 0 para subir
	TCCR1B |= (1 << COM1B1);	//Activa la salida del PWM
	TCCR1B &= ~(1 << COM1B0);	//Activa la salida del PWM
	motor_carga=1;	//Avisa de que el motor esta encendido
}

void bajar_carga(){
	PORTK &= ~(1 << PK7); //Dir M1 es K7, lo pone a 0 para bajar
	TCCR1B |= (1 << COM1B1);	//Activa la salida del PWM
	TCCR1B &= ~(1 << COM1B0);	//Activa la salida del PWM
	motor_carga=1;	//Avisa de que el motor esta encendido
}

int main(void){	//Ejemplo de uso
    setup_carga_retorno();
	setup_rebotes();
	subir_retorno();
	motor_retorno_status=1;
	while(motor_retorno_status==1){
		motor_retorno_status = leer_motor_retorno();
	}
	bajar_retorno();
	while(motor_retorno_status==1){
		motor_retorno_status = leer_motor_retorno();
	}
	subir_carga();
	motor_carga_status=1;
	while(motor_carga_status==1){
		motor_carga_status = leer_motor_carga();
	}
	bajar_carga();
	while(motor_carga_status==1){
		motor_carga_status = leer_motor_carga();
	}
}
