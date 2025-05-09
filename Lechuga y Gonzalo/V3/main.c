#include "lanzador.h"

int main(void){	//Ejemplo de uso
    setup_carga_retorno();
	setup_rebotes();
	puesta_cero();
	motor_carga_status=1;	//Esto esta aqui porque si no no furula para llamar a ceros y luego seguido a las funciones, luego en el codigo no haria falta
	while((motor_carga_status==1)||(motor_retorno_status==1)){
			motor_carga_status = leer_motor_carga();
		        motor_retorno_status = leer_motor_retorno();
		}
	ciclo_retorno();
	ciclo_carga();
}
