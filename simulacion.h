#include "directorios.h"
#include "semaforo_mutex_posix.h"
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>  
#define REGMAX 500000
#define NUMPROCESOS 100
#define NUMESCRITURAS 50 // a partir de 33 da error 33 incluido
sem_t mutex;

struct REGISTRO {
    time_t fecha; //fecha de la escritura en formato epoch 
    pid_t pid; //PID del proceso que lo ha creado
    int nEscritura; //Entero con el número de escritura (de 1 a 50)
    int nRegistro; //Entero con el número del registro dentro del fichero 
};

void reaper();