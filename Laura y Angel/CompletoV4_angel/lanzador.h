/*
 * lanzador.h
 *
 * Created: 06/05/2025 23:44:05
 *  Author: ASUS
 *
 *	aquí vamos a incluir todas las variables / funciones / defines de los .c del lanzador
 */ 


#ifndef LANZADOR_H_
#define LANZADOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define EN2 PL3
#define EN3 PL4
#define EN4 PL5
#define LED PL6

#define PWM2 8192
#define PWM3 32768
#define PWM4 32768

#define DIR2 PL0
#define DIR3 PL1
#define DIR4 PL2

#define SW2 PB2
#define SW3 PB3
#define SW4 PB4

void setup();

void brazoDerecha();
void brazoIzquierda();
void brazoParar();
void vastagoArriba();
void vastagoAbajo();
void vastagoParar();
void barreraArriba();
void barreraAbajo();
void barreraParar();

void ceroLanzador();
void millisLanzador();
void antirrebotesLanzador(uint8_t pin_bit);

#endif /* LANZADOR_H_ */