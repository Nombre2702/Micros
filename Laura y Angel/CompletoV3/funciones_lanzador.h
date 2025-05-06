#ifndef FUNCIONES_LANZADOR_H_
#define FUNCIONES_LANZADOR_H_

#define SW2 PB2
#define SW3 PB3
#define SW4 PB4

#define DIR2 PL0
#define DIR3 PL1
#define DIR4 PL2

#define PWM2 32768
#define PWM3 32768
#define PWM4 32768

#define DEBOUNCE_TIME 20 //ms

#include <avr/io.h>
#include <avr/interrupt.h>

// variables compartidas
extern volatile int comienzo;
extern volatile int cuenta_comienzo;

extern volatile uint8_t contador_cero_plat;
extern volatile uint8_t contador_cero_vast;
extern volatile uint8_t contador_cero_barr;
extern volatile uint8_t contador_parpadeo_led;

extern volatile uint8_t contador_sw2;
extern volatile uint8_t contador_sw3;
extern volatile uint8_t contador_sw4;
extern volatile uint8_t sentido_plataforma;
extern volatile uint32_t timer4_millis;
extern volatile uint32_t last_time_sw2;
extern volatile uint32_t last_time_sw3;
extern volatile uint32_t last_time_sw4;

// funciones
void unMs(void);

void setupMillis(void);
uint32_t millis(void);

void setupLanzador(void);

void pararPlataforma(void);
void rotarIzquierda(void);
void rotarDerecha(void);
void ceroPlataforma(void);
void balanceoLanzador(void);

void moverAdelante(void);
void moverAtras(void);
void pararVastago(void);
void ceroVastago(void);

void disparo(void);

void barreraArriba(void);
void barreraAbajo(void);
void origenBarrera(void);

void ledOn(void);
void ledOff(void);
void ledParpadeo(void);

#endif