#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 3){
		printf("Sintaxis: mi_rm <disco> <ruta> \n");
		return -1;
	}
	bmount(argv[1]);
	
       if(mi_unlink(argv[2])==-1){
		   return-1;
	   }
	   bumount();
	}
	
		
	

	
