#include "directorios.h"

int main(int argc, char **argv){
    int permisos;
    char *ruta;
   if(argc!=4){
       printf("Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
       return -1;
   }
   bmount(argv[1]);
   permisos= atoi(argv[2]);
   ruta= argv[3];
    if(permisos<0 || permisos>7){
        fprintf(stderr,"ERROR: permiso no válido: <<%d>>\n",permisos);
        return -1;
    }
    if(ruta[strlen(ruta)]=='/'){
        if(mi_creat(ruta,permisos)==-1){
            return -1;
        }
    }
    if (mi_creat(ruta,permisos)<0) {
       return -1;
    }
    bumount();
}