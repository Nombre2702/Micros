#ifndef RETORNO_H_
#define RETORNO_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint8_t boton;
int getBoton();

void finalCarga();
void finalRetorno(); 

void setup_rebotes();
void setup_carga_retorno();
void finalCarga();
void finalRetorno();
int leer_motor_retorno();
int leer_motor_carga();
int getBoton();
void subir_retorno();
void bajar_retorno();
void subir_carga();
void bajar_carga();
void retorno();
void carga();
void ceroRetorno();

#endif /* RETORNO_H_ */
