/*
 * parpadeoLed.h
 *
 * Created: 08/05/2025 15:29:06
 *  Author: ASUS
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>

#ifndef PARPADEOLED_H_
#define PARPADEOLED_H_

#define LED PL6

void parpadeoLed();
void ledOn();
void ledOff();
void millisLanzador();
void setup();

#endif /* PARPADEOLED_H_ */