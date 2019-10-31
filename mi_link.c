#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 4){
		printf("Sintaxis: mi_link <disco> <ruta_fichero> <ruta_enlace> \n");
		return -1;
	}
	bmount(argv[1]);
	if(mi_link(argv[2], argv[3]) == 0){
		printf("Enlace creado correctamente \n");
	} else {
		printf("Error en mi_link \n");
	}

	bumount();
	return 0;
}