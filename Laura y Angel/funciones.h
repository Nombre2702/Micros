#include "funciones_elementales.h"

extern volatile uint8_t flag_b ;
extern volatile uint8_t flag_30s;
extern volatile uint8_t flag_fin;
extern volatile uint8_t flag ;
extern volatile uint8_t sentido_giro ;
extern volatile uint8_t contador ;

void inicio();
void cargar_bola();
void cabeceo();
void cargar_tiro();
void disparo(int contador_final_carrera_3);
void balanceoLanzador(int contador_final_carrera_2, int sentido_giro);
