#include "bloques.h"
#include "semaforo_mutex_posix.h"
static int descriptor = 0;
static unsigned int inside_sc = 0;
static sem_t *mutex;

int bmount(const char *camino) {
   if (descriptor > 0) {
       close(descriptor);
   }
   if ((descriptor = open(camino, O_RDWR | O_CREAT, 0666)) == -1) {
      fprintf(stderr, "Error: bloques.c → bmount() → open()\n");
   }
   if (!mutex) { //mutex == 0
   //el semáforo es único y sólo se ha de inicializar una vez en nuestro sistema (lo hace el padre)
       mutex = initSem(); //lo inicializa a 1
       if (mutex == SEM_FAILED) {
           return -1;
       }
   }
   return descriptor;
}

int bumount() {
   descriptor = close(descriptor); 
   // hay que asignar el resultado de la operación a la variable ya que bmount() la utiliza
   if (descriptor == -1) {
       fprintf(stderr, "Error: bloques.c → bumount() → close(): %d: %s\n", errno, strerror(errno));
       return -1;
   }
   deleteSem(); // borramos semaforo 
   return 0;
}


void mi_waitSem(){
if (!inside_sc) {
waitSem(mutex);
}
inside_sc++;
}
void mi_signalSem() {
inside_sc--;
if (!inside_sc) {
    signalSem(mutex);
}
}


int bwrite(unsigned int nbloque, const void *buf)
{
    //size_t nbytes;
        lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    /* nbytes = write(descriptor, buf, BLOCKSIZE);
     if(nbytes==-1){
         perror("Error de escritura");
     }*/
    return write(descriptor, buf, BLOCKSIZE);
}
int bread(unsigned int nbloque, void *buf)
{
    //size_t nbytes;
    lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    /* nbytes=read(descriptor, buf, BLOCKSIZE);
    if(nbytes==-1){
        perror("Error de lectura");
    }*/
    return read(descriptor, buf, BLOCKSIZE);

}
