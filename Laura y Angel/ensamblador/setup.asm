

.include "m640def.inc"
 // Definición de pines 

 //pines dirección
.equ DIR2 = 0      ; PL0
.equ DIR3 = 1      ; PL1
.equ DIR4 = 2      ; PL2

//pines pwm
.equ EN2  = 3      ; PL3
.equ EN3  = 4      ; PL4
.equ EN4  = 5      ; PL5

//led
.equ LED  = 6      ; PL6

//pines entradas
.equ SW2  = 2      ; PB2
.equ SW3  = 3      ; PB3 
.equ SW4  = 4      ; PB4 

//pwm

.equ PWM2 = 16384
.equ PWM3 = 32768
.equ PWM4 = 49152


setup:

	//entradas
    ldi r16, 0
    out DDRB, r16
    ldi r16, (1<<SW2)|(1<<SW3)|(1<<SW4)
    out PORTB, r16

    //salidas
    ldi r16, (1<<DIR2)|(1<<DIR3)|(1<<DIR4)|(1<<EN2)|(1<<EN3)|(1<<EN4)|(1<<LED)
    out DDRL, r16        
    out PORTL, r16       

    //interrupciones
    ldi r16, (1<<PCIE0)
    sts PCICR, r16

    ldi r16, (1<<SW2)|(1<<SW3)|(1<<SW4)
    sts PCMSK0, r16     

	//timer 4
    
    ldi r16, 0
	sts TCCR4A, r16

	ldi r16, (1 << WGM42)
	sts TCCR4B, r16

	//prescaler
	ldi r16, (1 << CS41)
	sts TCCR4B, r16

	; OCR4A = 999 = 0x03E7
	ldi r16, LOW(999)
	sts OCR4AL, r16
	ldi r17, HIGH(999)
	sts OCR4AH, r17

	//int. por comp
	ldi r16, (1 << OCIE4A)
	sts TIMSK4, r16

	//no se si hace falta
	ldi r16, 0
	sts TCNT4L, r16
	sts TCNT4H, r16

	//timer 5

	ldi r16, (1 << WGM51)
	sts TCCR5A, r16

	ldi r16, (1 << WGM52) | (1 << WGM53)
	sts TCCR5B, r16

	//prescaler
	ldi r16, (1 << CS51)
	sts TCCR5B, r16

	//top de icr5 0xffff
	ldi r16, 0xFF
	ldi r17, 0xFF
	sts ICR5L, r17
	sts ICR5H, r16


	//pwm
	ldi r16, LOW(PWM2)
	sts OCR5AL, r16
	ldi r17, HIGH(PWM2)
	sts OCR5AH, r17

	ldi r16, LOW(PWM3)
	sts OCR5BL, r16
	ldi r17, HIGH(PWM3)
	sts OCR5BH, r17

	ldi r16, LOW(PWM4)
	sts OCR5CL, r16
	ldi r17, HIGH(PWM4)
	sts OCR5CH, r17

  
