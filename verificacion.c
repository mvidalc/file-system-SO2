#include "bloques.h"
#include <stdio.h>
#include "verificacion.h"

int main(int argc, char **argv){
    struct STAT stat;
    char informe[128];
    int inf = 0;

    if(argc != 3){
        printf("Error:sintaxis: ./verificacion <disco> <directorio_simulacion>\n");
    }
    if(bmount(argv[1])==-1){
		fprintf(stderr, "Error:\n");
		return -1;
	}
 	if(mi_stat(argv[2], &stat)==-1){
		fprintf(stderr,"Error: \n");
	}
        
	fprintf(stderr,"dir_sim: %s\n",argv[2]);
    int nentradas = stat.tamEnBytesLog/ sizeof(struct entrada);
	fprintf(stderr,"numentradas: %i ",nentradas);
    fprintf(stderr,"NUMPROCESOS: %d\n", NUMPROCESOS);

	sprintf(informe, "%s%s", argv[2], "informe.txt");
	if(mi_creat(informe, 7)==-1){
		fprintf(stderr,"Error\n");
		exit(EXIT_FAILURE);
	}

    struct entrada entradas[nentradas];        
	if (mi_read(argv[2], entradas, 0, sizeof(entradas)) == -1) {
		fprintf(stderr,"Error:\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < nentradas; i++){
        pid_t pid = atoi(strchr(entradas[i].nombre, '_') + 1);
        int num_escrituras = 0;
		struct REGISTRO primeraE;
		struct REGISTRO ultimaE;
		struct REGISTRO menorP;
		struct REGISTRO mayorP;
            
        char fprueba[128];
		sprintf(fprueba, "%s%s/prueba.dat", argv[2], entradas[i].nombre); 
		int offset = 0;
		struct REGISTRO registros[BLOCKSIZE/sizeof(struct REGISTRO)];
	    while(mi_read(fprueba, registros, offset, sizeof(registros)) > 0) {
			for (int j = 0; j < BLOCKSIZE/sizeof(struct REGISTRO); j++) {
				if (registros[j].pid == pid) {
					if (num_escrituras == 0) {
						primeraE = registros[j];
						ultimaE = registros[j];
						menorP = registros[j];
						mayorP = registros[j];
					}else {
						if (registros[j].nEscritura < primeraE.nEscritura) {
							primeraE = registros[j];
						} else if (registros[j].nEscritura > ultimaE.nEscritura) {
							ultimaE = registros[j];
						}
						if (registros[j].nRegistro < menorP.nRegistro) {
							menorP = registros[j];
						} else if (registros[j].nRegistro > mayorP.nRegistro) {
							mayorP = registros[j];
						}
					}
					num_escrituras++;
				}
			}
			memset(registros, 0, sizeof(registros));
			offset += sizeof(registros);
		}
		printf("%i) %i escrituras validadas en %s\n",(i+1), num_escrituras, fprueba);
		char buffer[1024];
		memset(buffer, 0, 1024);
		sprintf(buffer, "\nPID: %i\nNumero de escrituras: %i\n", pid, num_escrituras);
		sprintf(buffer + strlen(buffer), "Primera escritura:	%i	%i	%s",
		primeraE.nEscritura,		
		primeraE.nRegistro,
		asctime(localtime(&primeraE.fecha)));
		sprintf(buffer + strlen(buffer), "Ultima escritura:	%i	%i	%s",
		ultimaE.nEscritura,
		ultimaE.nRegistro,
		asctime(localtime(&ultimaE.fecha)));
		sprintf(buffer + strlen(buffer), "Menor posicion:		%i	%i 	%s",
		menorP.nEscritura,
		menorP.nRegistro,
		asctime(localtime(&menorP.fecha)));
		sprintf(buffer + strlen(buffer), "Mayor posicion:		%i	%i	%s",
		mayorP.nEscritura,
		mayorP.nRegistro,
		asctime(localtime(&mayorP.fecha)));
		mi_write(informe, buffer, inf, strlen(buffer));
		inf += strlen(buffer); 
    }

	if(bumount()==-1){
		fprintf(stderr, "Error: verificacion.c -> llamada a bumount \n");
		return -1;
	}
	    return 0;
}
    