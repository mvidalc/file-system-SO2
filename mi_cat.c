#include "directorios.h"

int main(int argc, char **argv) {
	if(argc!=3){
		fprintf(stderr,"Sintaxis: mi_cat <nombre_dispositivo> </ruta_fichero>\n");
		return -1;
	}
	int lruta = strlen(argv[2]);
	if(argv[2][lruta-1]== '/'){
        fprintf(stderr,"Error la ruta no puede ser un directorio.\n");
        return -1;
    }
	bmount(argv[1]);
	unsigned char buff[4*BLOCKSIZE];
	memset(buff,0,4*BLOCKSIZE);
	int bytesLeidos= 0;
	int totalLeidos = 0;
	int ibyte = 0;
	while((bytesLeidos = mi_read(argv[2],buff,ibyte,4*BLOCKSIZE))>0){
		if(write(1,buff,bytesLeidos)== -1){
			perror("Error\n");
			return -1;
		}
		memset(buff,0,4*BLOCKSIZE);
		ibyte += bytesLeidos;
		totalLeidos += bytesLeidos;

	}
	fprintf(stderr,"\nBytes leidos: %d\n", totalLeidos);
	bumount();
	return 0;

}
