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
	int bytesEscritos=0;
    bmount(argv[1]);
	for(int i=0;i<10;i++){
      bytesEscritos = bytesEscritos + mi_write(argv[2], txt, offset, l);
      offset=offset+BLOCKSIZE;
    }
  
	
    printf("Bytes escritos: %d\n", bytesEscritos);
    bumount();
    return 0;
}