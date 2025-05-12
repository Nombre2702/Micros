#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <stdbool.h>

#define T_PARTIDA 15000
#define T_FINALPISTA 2500
#define T_RAMPA 1000
#define T_DISPLAY 128 // T = 2^^n

#define T_DEBUG 500

typedef enum {
    GAME,
    GAME_FINAL,
    STANDBY
} gameState;

typedef enum {
    SET_CERO,
    LOADING,
    READY
} machineState;

typedef enum {
    POS_UNKNOWN,
    POS_CARGA,
    POS_CABECEO
} lanzadorState;




// MARK: Global vars
int timer_global;
int timer_sync;
int timer_emulador;

int pin_count;
int pin_count_display;

bool carga_0;
bool retorno_0;

bool didReload;
bool didLift;

gameState game_state;
machineState machine_state;
lanzadorState lanzador_state;


/* Setters */

void check_carga_0(void) {
    carga_0 = true;
}
void check_retorno_0(void) {
    retorno_0 = true;
}

void setMachine_SET_CERO(void) {
    machine_state = SET_CERO;
}
void setMachine_LOADING(void) {
    machine_state = LOADING;
}
void setMachine_READY(void) {
    machine_state = READY;
}
void setGame_GAME(void) {
    game_state = GAME;
}
void setGame_GAME_FINAL(void) {
    game_state = GAME_FINAL;
}
void setGame_STANDBY(void) {
    game_state = STANDBY;
}

void setLanzador_CARGA(void) {
    lanzador_state = POS_CARGA;
}
void setLanzador_CABECEO(void) {
    lanzador_state = POS_CABECEO;
}

// MARK: millis();

void millis(unsigned int ms) {
    usleep(ms * 1000);
}

// MARK: Sim. AL: f(x:) Con callbacks @ T = 1000ms
void setupLanzador(void) {
    printf("setupLanzador(); \n");
} // No callback, pin set-up (just once).
void ceroLanzador(void (*callback)(void)) {
    printf("setupLanzador(); \n");
    millis(T_DEBUG);
    callback();
} // P @ cero del lanzador; cb() pasar de SET_CERO a LOADING
void cabeceo(void) {
    return;
} // cb() --> Listo para cargar disparo
void cargarDisparo(void (*callback)(void)) {
    printf("cargaDisparo(); \n");
    millis(T_DEBUG);
    callback();
} // cb() --> Final. cargar las gomas
void prepararCarga(void (*callback)(void)) {
    printf("prepararCarga(); \n");
    millis(T_DEBUG);
    callback();
} // Volver a la dcha, TRAS DISPARO, cb() --> llego
void disparo(void (*callback)(void)) {
    printf("disparo(); \n");
    millis(T_DEBUG);
    callback();
} // cb() --> Sale la bola, pongo t_sync a 0.

void colocarCabeceo(void (*callback)(void)) {
    millis(T_DEBUG);
    callback();
}

void subir_retorno(void (*callback)(void)) {
    millis(T_DEBUG);
    callback();
}
    
void bajar_retorno(void (*callback)(void)) {
    millis(T_DEBUG);
    callback();
}
void subir_carga(void (*callback)(void)) {
    millis(T_DEBUG);
    callback();
}
void bajar_carga(void (*callback)(void)) {
    millis(T_DEBUG);
    callback();
}

void cargar(void (*callback)(void)) {
    millis(T_DEBUG);
    callback();
}
void check_didReload(void) {
    didReload = true;
}

void elevador(void) {
    
}


void setup(void) {
    
    /*
     Cfg de los pines, de LANZADOR, LED + BOLOS, RETORNO & CARGA
     */
    /*
     setupLanzador();
     
     setup_carga_rebotes();
     
     setup_rebotes();
     */
    machine_state  = SET_CERO;
    game_state     = STANDBY;
    lanzador_state = POS_UNKNOWN;
    
    /*
     Como sólo tengo que hacer la puesta a cero del RETORNO y CARGA 1 vez,
     y teniendo sólo funciones que retornan a este mismo estado, uso 2 booleanas
     para comprobar si la puesta a cero está hecha o no.
     No me interesan los estados de RETORNO y CARGA a lo largo de la ejecución,
     supongo que tras cargar() y elevador() VUELVEN a la misma posición del estado SET_CERO
     carga_0    = false
     retorno_0 = false
     */
    
    carga_0 = false;
    retorno_0 = false;
    
    timer_global = 0;
    timer_sync = 0;
    
    timer_emulador = 0;
    
    millis(T_DEBUG);
}




void startGame(void) {
    printf("startGame(), game: STANDBY --> GAME \n");
    game_state = GAME;
    timer_global = 0;
}

void endGame(void) {
    pin_count = 0;
    pin_count_display = 0;
    // displayNumber();
    
    game_state = STANDBY;
    /*
     La máquina se recarga, y queda en
     machine_state =
     */
}



void start_sync_timer(void) {
    timer_sync = 0;
}

void fire(void) {
    /*
     Hago el disparo, me avisa cuando la bola ha salido.
     Pongo el t_sync a cero, machine_state = LOADING, y ahora hago la lógica de la recarga
     */
    disparo(start_sync_timer);
    setMachine_LOADING();
}

void reload(void) {
    
    /*
     Preparo el LANZADOR; que vuelva a POS_CARGA
     
     */
    
    if (timer_sync == T_FINALPISTA) {
        elevador();
    }
    
    if (lanzador_state != POS_CARGA) {
        prepararCarga(setLanzador_CARGA);
        /*
         Si vengo del SET_CERO, ya debería estar en POS_CARGA
         Si no, es que vengo de tirar, así que lo pongo
         */
    } else {
        // Ahora tengo el lanzador en POS_CARGA, hago la CARGA;
        
        if (!didReload) {
            cargar(check_didReload);
        } else {
            // Ya he cargado, así que preparo cabeceo
            colocarCabeceo(setLanzador_CABECEO);
            if (lanzador_state == POS_CABECEO) {
                
                /*
                 Estoy ya con la bola cargada, y en posición de cabeceo.
                 Es decir, he hecho la recarga, reload() completado.
                 */
                
                machine_state = READY;
                didReload = false;
                printf("reload() complete. \n");
            }
        }
    }
}



void setCero(void) {
    /*
     Pongo:
        retorno    ABAJO  (Siempre así a no ser que se llame a elevador());
        carga      ARRIBA ("                                 " recarga());

        lanzador   POS_CARGA después de carga ARRIBA!
     */
    printf("setCero() called. \n");
    
    if (!carga_0) {
        subir_carga(check_carga_0);
    }
    if (!retorno_0) {
        bajar_retorno(check_retorno_0);
    }
    if (!carga_0) {
        subir_carga(check_carga_0);
    } else {
        // La CARGA está en la pos. ALTA, donde NO molesta a LANZADOR
        ceroLanzador(setLanzador_CARGA);
        if (lanzador_state == POS_CARGA) {
            // LANZADOR en POS_CARGA, RETORNO abajo, CARGA arriba, ya he hecho la puesta a 0.
            machine_state = LOADING;
            
            printf("setCero() complete. \n");
        }
    }
}



void timer(void) {
    
    
    if (game_state != STANDBY) {
        timer_sync++;
        timer_global++;
        
        if (timer_global == T_PARTIDA) {
            game_state = GAME_FINAL;
            printf("game final \n");
        }
        if (timer_sync == (T_FINALPISTA)) {
            /*
             Hago el cálculo de los puntos
             pin_count_display = pin_count;
             */
        }
    }
    
    
}

void boton(void) {
    
    if (machine_state == READY) {
        if (game_state == STANDBY) {
            startGame();
        } else {
            fire();
            if (game_state == GAME_FINAL) {
                endGame();
            }
        }
    }
    
}

void emulator(void) {
    
    /*
     Emular timer, sólo en CONSOLA, borrar todo lo referente a *** para probar en maqueta
     */
    
    millis(1);
    timer_emulador++;
    timer();
    
    if (timer_emulador == 5000) {
        printf("b \n");
        boton();
    }
    if (timer_emulador == 10000) {
        printf("b \n");
        boton();
    }
    if (timer_emulador == 20000) {
        printf("b \n");
        boton();
    }
    
}


// MARK: Máquina de estados
int main(int argc, const char * argv[]) {
    
    setup();
    while (1) {
        
        emulator();
        switch(machine_state) {
                
            case SET_CERO:
                setCero();
                break;
                
            case LOADING:
                reload();
                break;

            case READY:
                /* Los estados están vacíos, se p. borrar
                switch(game_state) {
                    case STANDBY:
      
                        break;
                    case GAME:
         
                        break;
                    case GAME_FINAL:
    
                        break;
                    default:
                        break;
                }
                 */
                break;

            default:
                break;
        }
        
    }
    return 0;
}
 

/*
 Cosas por hacer;
    CARGA Y RETORNO, integración.
 
    LEDS Y ESCUCHA DE PINES, integración (f(x) ya protot. falta probar)
 */
