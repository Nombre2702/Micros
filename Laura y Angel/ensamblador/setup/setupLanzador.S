
/*
 * setupLanzador.S
 *
 * Created: 14/05/2025 0:14:53
 *  Author: laura
 */ 
 #include <avr/io.h>
.global setupLanzador

setupLanzador:
	ldi r16, 0b11100011
    sts DDRB, r16
    sts PORTB, r16

	// Salidas: lanzador
    ldi r16, 0b01111111
    sts DDRL, r16            
    sts PORTL, r16

	// Interrupciones
    ldi r16, (1 << PCIE0)    
    sts PCICR, r16            
    ldi r16, 0b00011111  
    sts PCMSK0, r16  

	// Timer 4 
    ldi r16, 0
    sts TCCR4A, r16

    ldi r16, (1 << WGM42) | (1 << CS41) 
    sts TCCR4B, r16

    ldi r16, 0xE7
    ldi r17, 0x03
	sts OCR4AH, r17
    sts OCR4AL, r16            
    
	nop
	nop
	nop
	nop

    ldi r16, (1 << OCIE4A)    
    sts TIMSK4, r16
	           
    ldi r16, 0
	sts TCNT4H, r16
    sts TCNT4L, r16           
               

    // Timer 5
    ldi r16, (1 << COM5A1) | (1 << WGM11)
    sts TCCR5A, r16
    ldi r16, (1 << WGM13) | (1 << WGM12) | (1 << CS51)
    sts TCCR5B, r16

	lds r16, TIMSK5
	ori r16, (1 << OCIE5A)  ; Habilitar interrupción por comparación
	sts TIMSK5, r16

	ldi r16, 0x7D
    sts OCR5AH, r16
    ldi r16, 0x00
    sts OCR5AL, r16

	nop
	nop
	nop
	nop
	nop

	//PW3
	ldi r16, 0x7D    
    sts OCR5BH, r16
	ldi r16, 0x00      
    sts OCR5BL, r16    
	
	nop
	nop  
	nop
	nop     

	//PWM4
	ldi r16, 0x7D   
    sts OCR5CH, r16 
	ldi r16, 0x00       
    sts OCR5CL, r16 

	nop
	nop
	nop
	nop
