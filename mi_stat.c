#include "directorios.h"

int main(int argc, char **argv){

    if (argc != 3) {
        fprintf(stderr, "Syntax error: ./mi_stat <disco> </ruta>");
        return -1;
    }
    int ninodo,permisos;
	char tipo;
	char atime[80], mtime[80],cttime[80];
	struct tm *ts;
	struct STAT stat;
	bmount(argv[1]);	
	ninodo=mi_stat(argv[2],&stat);
	if(ninodo<0) return -1;
	ts = localtime(&stat.atime);
	strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&stat.mtime);
	strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&stat.ctime);
	strftime(cttime, sizeof(cttime), "%a %Y-%m-%d %H:%M:%S", ts);	
	tipo=stat.tipo;
	permisos = stat.permisos;
	printf("\ntipo: %c \npermisos: %d \n",tipo,permisos);
	printf("ATIME: %s \nMTIME: %s \nCTIME: %s \nnlinks: %d \ntamEnBytesLog: %d \nnumBloquesOcupados: %d \n\n",atime,mtime,cttime,stat.nlinks,stat.tamEnBytesLog,stat.numBloquesOcupados);
	bumount();
	return 0;
    
}
