#include "ficheros.h"

int main(int argc, char **argv){

	int tam = 1500;
	unsigned char buffAux[tam];
	unsigned int offset=0;
	char string[128];
	struct STAT stat;
	int acumulado = 0;
	
	int leidos;
	memset(buffAux, 0, tam);
	

	if(argc != 3){

		fprintf(stderr, "Sintaxis: leer <nombre_dispositivo> <nºinodo>\"\n");
		return -1;

	}

	bmount(argv[1]);

	while((leidos = mi_read_f(atoi(argv[2]), buffAux,offset, tam)) > 0){

		write(1, buffAux, leidos);
		memset(buffAux, 0, tam);
	
		acumulado = leidos + acumulado;
		offset = offset + tam;
	

	}

	mi_stat_f(atoi(argv[2]), &stat);
	sprintf(string, "bytes leídos %d\n", acumulado);
	write(2, string, strlen(string));
	printf("Tamaño en bytes lógicos: %d\n", stat.tamEnBytesLog);
	bumount();

}