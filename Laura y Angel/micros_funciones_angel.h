#ifndef MICROS_FUNCIONES_ANGEL_H_
#define MICROS_FUNCIONES_ANGEL_H_

#define DEBOUNCE_TIME 20 //ms

#include <avr/io.h>

// variables compartidas
extern volatile uint8_t contador_aux;
extern volatile uint8_t contador_sw2;
extern volatile uint8_t contador_sw3;
extern volatile uint8_t contador_sw4;
extern volatile uint8_t sentido_plataforma;
extern volatile uint16_t duty_2;
extern volatile uint16_t duty_3;
extern volatile uint16_t duty_4;

// funciones
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

#endif