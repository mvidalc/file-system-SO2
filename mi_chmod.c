#include "directorios.h"


int main(int argc, char **argv){
    int permisos;
    if (argc != 4) {
        fprintf(stderr, "Syntax error: ./mi_chmod <nombre_dispositivo> <permisos> </ruta>\n");
        return -1;
    }
    permisos = atoi(argv[2]);

    if (permisos > 7 || permisos < 0) {
        fprintf(stderr, "Error:  modo inválido: <<%d>>\n",permisos);
        return -1;
    }
	bmount(argv[1]);
	if((mi_chmod(argv[3],permisos))== -1){
        fprintf(stderr,"Error de permisos\n");
       return -1; 
    }
	bumount();
	return 0;   
}