#include "simulacion.h"
static int acabados = 0; 

int main(int argc, char **argv){
    struct REGISTRO registro;
    int pid;
    if (argc != 2) {
        fprintf(stderr, "Syntax error: ./simulacion <disco>");
        return -1;
    }
    if(bmount(argv[1])==-1){
        fprintf(stderr,"Error: simulacion.c -> bmount \n");
        return -1;
    }
    time_t now = time(NULL);
    struct tm *ts;
	char directorio[100];
    char fic[100];
    char diractual[100];
    memset(diractual, 0, 100);
	time(&now);
	ts = localtime(&now);
	strftime(directorio, sizeof(directorio), "/simul_%Y%m%d%H%M%S/", ts);
	if(mi_creat(directorio, 7)<0){
        if(bumount()==-1){
            fprintf(stderr,"Error: simulacion.c -> bumount \n");
            return -1;
        } 
        return -1;
    } 
    signal(SIGCHLD, reaper);
    int proceso;
    fprintf(stderr,"*** Simulación de %d procesos realizando cada uno %d escrituras ***\n",NUMPROCESOS,NUMESCRITURAS);
    fprintf(stderr,"Directorio simulación: %s\n",directorio);
    for(proceso = 0; proceso < NUMPROCESOS; proceso++) {
        pid=fork();
        if(pid == 0){
            if(bmount(argv[1])==-1){
                fprintf(stderr,"Error: simulacion.c -> bmount \n");
                return -1;
            }
		    sprintf(diractual, "%sproceso_%d/", directorio, getpid());
			if(mi_creat(diractual, 7)<0){
				if(bumount()==-1){
                    fprintf(stderr,"Error: simulacion.c -> bumount \n");
                    return -1;
                }  
				exit(EXIT_FAILURE);
			}
			sprintf(fic, "%sprueba.dat", diractual);
			if(mi_creat(fic, 7)<0){ 
				if(bumount()==-1){
                    fprintf(stderr,"Error: simulacion.c -> bumount \n");
                    return -1;
                } 
				exit(EXIT_FAILURE);
			}
            printf("Proceso %i) Realizadas %d escrituras en %s\n",proceso+1,NUMESCRITURAS,fic);
            srand((time(NULL)+getpid()));
            int i;
            for(i = 0; i < NUMESCRITURAS; i++) {
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = i + 1;
                registro.nRegistro = rand()%REGMAX;
                if(mi_write(fic, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO))==-1){
                    fprintf(stderr,"Error: simulacion.c -> llamada a mi_write\n");
                    return -1;
                }
                //fprintf(stderr,"[simulación.c → Escritura %d en %s]\n",registro.nEscritura,fic);
                usleep(50000);
            }
            if(bumount()==-1){
                fprintf(stderr,"Error: simulacion.c -> bumount \n");
                return -1;
            } 
            exit(0);     
        }else if(pid<0){
            fprintf(stderr,"Error: simulación.c -> Error PID < 0 \n");
            exit(EXIT_FAILURE);
        }
        usleep(200000);
    }
    while(acabados < NUMPROCESOS) {
        pause();
    }
    fprintf(stderr, "Total de procesos terminados: %d\n",acabados);
    bumount();
    exit(0);
}

void reaper(){
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended=waitpid(-1, NULL, WNOHANG)) > 0) {
        acabados++;
        //Podemos testear qué procesos van acabando:
       // fprintf(stderr, "[simulación.c → Acabado proceso con PID: %d, total acabados: %d]\n", ended, acabados);
    }
}