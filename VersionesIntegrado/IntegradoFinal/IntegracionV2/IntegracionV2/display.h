/*
 * display.h
 *
 * Created: 13/05/2025 17:04:07
 *  Author: jesus
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_	


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init_display();
void timerDisplay();
void mostrar_digito(uint8_t digit, uint8_t select);

void mostrar_numero(uint8_t num);

#endif