#include <avr/io.h>
#include <avr/interrupt.h>
#include "funciones.h"
#define NO_REBOTE 50 //redefinir

volatile uint8_t flag_b = 0;
volatile uint8_t flag = 0;
volatile uint8_t flag_fin = 0;
volatile uint8_t sentido_giro = 0;
volatile uint8_t contador = 0;

//antirrebotes
volatile uint8_t ultimo_flanco = 0;
volatile uint8_t msegundos = 0; 

void setupLanzador(){

	// FINALES DE CARRERA
	// config entradas switches | puerto K
	DDRK &= ~((1 << PK3) | (1 << PK2) | (1 << PK1) | (1 << PK0));
	//interrupciones
	PCICR |= (1 << PCIE2);
	//máscaras no sé si son globales creo que sí y no sé si me están sirviendo para algo
	//ACTIVAR MÁSCARAS. estaba mal?
	PCMSK2 |= (1 << PCINT19) | (1 << PCINT18) | (1 << PCINT17) | (1 << PCINT16);

	// DIRECCIONES
	// config salidas direcciones | puerto L
	DDRL |= (1 << PL2) | (1 << PL1) | (1 << PL0);

	// ENABLES
	// setup del timer 5 | puerto L | Fast PWM
	// config salidas puerto L con enmascaramiento
	DDRL |= (1 << PL5) | (1 << PL4) | (1 << PL3);
	// config salida led
	DDRL |= (1 <<PL6);

	// config Fast PWM con ICR5 (1 1 1 0)
	TCCR5A &= ~(1 << WGM50);
	TCCR5A |= (1 << WGM51);
	TCCR5B |= (1 << WGM52);
	TCCR5B |= (1 << WGM53);


	// config prescaler (8)
	TCCR5B |= (1 << CS51);
	TCCR5B &= ~((1 << CS50) | (1 << CS52));

	// config output compares non-inverted (1 0)
	TCCR5A |= (1 << COM5A1); TCCR5A &= ~(1 << COM5A0);
	TCCR5A |= (1 << COM5B1); TCCR5A &= ~(1 << COM5B0);
	TCCR5A |= (1 << COM5C1); TCCR5A &= ~(1 << COM5C0);

	// config del TOP | ICR5 | es un registro de 16 bits asi que para aprovecharlo al maximo lo ponemos a 0xFFFF
	ICR5 = 0xFFFF;


}

//consulta periódica

volatile uint8_t sw2 = 0;
volatile uint8_t sw3 = 0;
volatile uint8_t sw4 = 0;
volatile uint8_t disp = 0;



//INTERRUPCIONES
//igual debería pasar el sentido de giro por referencia?

ISR(PCINT2_vect) {
    if (PINK & (1 << PK0)) sw2 = 1;
    if (PINK & (1 << PK1)) sw3 = 1;
    if (PINK & (1 << PK2)) sw4 = 1;
    if (PINK & (1 << PK3)) disp = 1;
}

void loop (){
    if (sw2){
        if (PINK & (1 << PK0)) {
            if ((msegundos - ultimo_flanco) < NO_REBOTE) {
                ultimo_flanco = msegundos;
            }else {
                if(flag_b){
                    balanceoLanzador(contador, sentido_giro);
                }else  {
                    sentido_giro =! sentido_giro;
                    giro(sentido_giro);
					msegundos = 0;
                }
            }sw2=0;
        }
    }

    if (sw3){
        if (PINK & (1 << PK1)) {
            if ((msegundos - ultimo_flanco) < NO_REBOTE) {
                    ultimo_flanco = msegundos;
            }else {
                PORTL &= ~(1 << PL4);
                sw3=0;
				msegundos = 0;
            }
        }
    }

    if (sw4){
        if (PINK & (1 << PK2)) {
            if ((msegundos - ultimo_flanco) < NO_REBOTE) {
                    ultimo_flanco = msegundos;
            }else {
                PORTL &= ~(1 << PL5);
                sw4=0;
				msegundos = 0;
            }
        }
    }


    if(disp){
        if (PINK & (1 << PK3)) {
            if ((msegundos - ultimo_flanco) < NO_REBOTE) {
                ultimo_flanco = msegundos;
            }//else disparo(fin_de_carrera); //REVISAR
        }
    }

}



//estoy asumiendo que el timer no lo controlo yo. no sé si puedo hacer esto?
ISR(TIMER5_COMPA_vect) {
	//flag_30s = 1;  //REVISAR
	msegundos++;
}


main(){
	cli();
	setupLanzador();
	sei();

	while(1){
            inicio();
            cargar_bola();
            if(flag_b){
                balanceoLanzador(contador, sentido_giro);
            }
        
	}
}




//pendiente hacer lo de loss timers y los pwm
//poner el pwm del m3 al máximo cuando se dispare
//configurar último tiro
//unificar llamadas a las funciones
//esquema de que tienen que hacer los de integración
//pasar mapa de pines cuando lo tenga

//no sé muy bien como reiniciar todo el juego





// reconfigurar 