//DISPLAY

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Tabla de segmentos (a-g) para los números del 0 al 9

const uint8_t segment_map[11] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111,  // 9
	0b00000000	// todo apagado
};

// Inicializa el puerto D para los displays

void init_display() {
	DDRD = 0xFF;             // Todos los pines del puerto D como salida (1)
	PORTD &= ~(1 << PD7);    // PD7 en 0 inicialmente (selector del display izquierdo)
}

// Muestra un dígito en el display seleccionado

void mostrar_digito(uint8_t digit, uint8_t select) {
	
	if (digit > 9) 
		PORTD = 0b00000000;		
	
	else
		PORTD = (PORTD & 0b10000000) | segment_map[digit]; // Cargar segmentos a–g en PD0–PD6

		// Selección del display (PD7)
	
		if (select == 0)
			PORTD &= ~(1 << PD7);  // Seleccionar display izquierdo
		
		else
			PORTD |= (1 << PD7);   // Seleccionar display derecho
}

// Multiplexa dos dígitos (0–99)

void mostrar_numero(uint8_t num) {
	
		uint8_t tens = num / 10;
		uint8_t ones = num % 10;
	
	if (num > 99){
		mostrar_digito(10, 1);   // Mostrar decenas
		_delay_ms(10);
		
		mostrar_digito(10, 0);   // Mostrar unidades
		_delay_ms(10);
	}
	else

		mostrar_digito(tens, 1);   // Mostrar decenas
		_delay_ms(10);
	
		mostrar_digito(ones, 0);   // Mostrar unidades
		_delay_ms(10);
}


// Función principal

int main(void) {
	init_display();

	

	while (1) {
		
		uint8_t num = 69;
		
		mostrar_numero(num);
		
		
		
		

		
	}
}
