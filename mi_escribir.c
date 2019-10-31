#include "directorios.h"


int main(int argc, char **argv){

    if (argc != 5) { // miramos si hay error de sintaxi
        fprintf(stderr, "Syntax error: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return -1;
    }
    int lruta = strlen(argv[2]);
    if(argv[2][lruta-1]== '/'){
        fprintf(stderr,"Error la ruta no puede ser un directorio.\n");
        return -1;
    }
    unsigned int offset = atoi(argv[4]);
	char *txt = argv[3];
	long l = strlen(txt);
	printf("Longitud texto: %li\n",l);
	int bytesEscritos;
    bmount(argv[1]);
	
    bytesEscritos = mi_write(argv[2], txt, offset, l);
	if (bytesEscritos == -1) {
	   // fprintf(stderr, "Error al escribir con la función mi_write\n");
	    bytesEscritos = 0;
        return -1;
	}
    printf("Bytes escritos: %d\n", bytesEscritos);
    bumount();
    return 0;
}
