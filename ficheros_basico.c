#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
    int n=(nbloques/8)/BLOCKSIZE;
    if((nbloques/8)%BLOCKSIZE==0){
        return n;;
    }else{
        return n+1;
    }
}

int tamAI(unsigned int ninodos){
    int n=(ninodos*INODOSIZE)/BLOCKSIZE;
    if((ninodos*INODOSIZE)%BLOCKSIZE==0){
        return n;
    }else{
        return n+1;
    }
}

int initSB(unsigned int nbloques, unsigned int ninodos){
    struct superbloque SB; 

    //Posición del primer bloque del MB
    SB.posPrimerBloqueMB = posSB + tamSB; //posSB = 0, tamSB = 1
    //Posición del último bloque del MB
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    //Posición del primer bloque del AI
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    //Posición del último bloque del AI
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI  + tamAI(ninodos) -1;
    //Posición del primer bloque de datos
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    //Posición del último bloque de datos 
    SB.posUltimoBloqueDatos = nbloques - 1;
    //Posición del inodo del directorio raíz en el AI
    SB.posInodoRaiz = 0; 
    //Posición del primer inodo libre en el AI
    SB.posPrimerInodoLibre = 0;
    //Cantidad de bloques libres del SF
    SB.cantBloquesLibres = nbloques;
    //Cantidad de inodos libres del AI
    SB.cantInodosLibres = ninodos;
    //Cantidad total de bloques del SF
    SB.totBloques=nbloques;  
    //Cantidad total de inodos del SF
    SB.totInodos=ninodos; 
    if(bwrite(posSB,&SB)<0){
        printf("initSB()-> Ha habido un error\n");
        return -1;
    }else{
        return 0;
    }
}

int initMB(){
    struct superbloque SB;
    unsigned char buff[BLOCKSIZE];
    if(memset(buff,0,BLOCKSIZE)<0){
        fprintf(stderr,"Error initMB() %d: %s\n",errno,strerror(errno));
        return -1;
    }
    if(bread(posSB,&SB)<0){
         printf("initMB()-> Ha habido un error\n");        
        return -1;
    }    
    for(int i=SB.posPrimerBloqueMB; i<SB.posUltimoBloqueMB ;i++){
        if(bwrite(i,buff)<0){
             printf("initMB()-> Ha habido un error\n");
             return -1;
        } 
    }

    for(int i=posSB; i<= SB.posUltimoBloqueAI;i++){
		escribir_bit(i,1);
        SB.cantBloquesLibres--;
    }    
  
    if(bwrite(posSB, &SB)<0){
        printf("initMB() -> Ha habido un error\n");
    }     

    return 0;
}

int initAI(){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    if(bread(posSB,&SB)<0){
        printf("initAI()-> Ha habido un error\n");
        return -1;
    }    
    int contInodos= SB.posPrimerInodoLibre+1;
    for(int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI;i++){
        for(int j=0; j<BLOCKSIZE / INODOSIZE; j++){
            inodos[j].tipo= 'l'; 
            if(contInodos < SB.totInodos){
                inodos[j].punterosDirectos[0]= contInodos;
                contInodos++;
            }else{
                inodos[j].punterosDirectos[0]= UINT_MAX;
            }
            
        }
        if(bwrite(i,inodos)<0){
             printf("initAI()-> Ha habido un error\n");
             return -1;
        }
    }    
    return 0;
}


int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
        printf("escribir_bit()-> Ha habido un error\n");
        return -1;
    }
    int posbyte= nbloque/8;
    int posbit=nbloque%8;
    int nbloqueMB= posbyte/BLOCKSIZE;
    int nbloqueabs= SB.posPrimerBloqueMB+nbloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    if(bread(nbloqueabs, bufferMB)<0){
         printf("escribir_bit()-> Ha habido un error\n");
         return -1;
    }     
    posbyte= posbyte%BLOCKSIZE;
    unsigned char mascara=128;
    mascara >>= posbit; //desplazar bits a la dcha
    switch(bit){
        case 1:
            bufferMB[posbyte] |=mascara; //operador OR para bits
        break;
        case 0:
            bufferMB[posbyte] &= ~ mascara; //operador AND y NOT para bits
        break;
    }
    if(bwrite(nbloqueabs, bufferMB)<0){
         printf("escribir_bit()-> Ha habido un error\n");
         return -1;
    } 
    return 0;
}

unsigned char leer_bit(unsigned int nbloque){
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
        printf("leer_bit() -> Ha habido un error\n");
        return -1;
    }
    int posbyte= nbloque/8;
    int posbit=nbloque%8;
    int nbloqueMB= posbyte/BLOCKSIZE;
    int nbloqueabs= SB.posPrimerBloqueMB+nbloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    if(bread(nbloqueabs, bufferMB)<0){
         printf("leer_bit()-> Ha habido un error\n");
         return -1;
    }
    posbyte= posbyte%BLOCKSIZE;
    unsigned char mascara=128;
    mascara >>= posbit; 
    mascara &= bufferMB[posbyte]; 
    mascara >>= (7-posbit); 

    printf("leer_bit -> nbloque: %d, posbyte: %d, posbit: %d\n", nbloque, posbyte, posbit);
    printf("leer_bit -> nbloqueMB: %d, nbloqueabs: %d\n", nbloqueMB, nbloqueabs);

    return mascara; 
}

int reservar_bloque(){
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
        printf("reservar_bloque() -> Ha habido un error\n");
        return -1;
    }
    int posBloqueMB =SB.posPrimerBloqueMB;
    int posbyte=0;
    int posbit=0; 
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];
    if(memset(bufferAux,255,BLOCKSIZE)<0){
        fprintf(stderr,"Error reservar_bloque() %d: %s\n",errno,strerror(errno));
        return -1;
    }
    if(SB.cantBloquesLibres > 0){ 
        if(bread(posBloqueMB, bufferMB)<0){
             printf("reservar_bloque()-> Ha habido un error\n");
             return -1;
        }    
        while(posBloqueMB <= SB.posUltimoBloqueMB && memcmp(bufferAux, bufferMB, BLOCKSIZE)==0){
            posBloqueMB++; 
            if(bread(posBloqueMB, bufferMB)<0){
                printf("reseravar_bloque()-> Ha habido un error\n");
                return -1;
            }
        }
        while(bufferMB[posbyte]>=255){
            posbyte++;
        }
        unsigned char mascara = 128; 
        if (bufferMB[posbyte] < 255) { 
	        while (bufferMB[posbyte] & mascara) {
		    posbit++;
            bufferMB[posbyte] <<= 1;
            }
        }   
    }else{
        printf("reservar_bloque() -> No hay bloques libres\n");                
    }

    int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
    escribir_bit(nbloque,1);
    SB.cantBloquesLibres--;
 
    if(bwrite(posSB, &SB)<0){
         printf("reservar_bloque()-> Ha habido un error\n");
         return -1;
    }     
    unsigned char bufferAux2[BLOCKSIZE];
    if(memset(bufferAux2,0,BLOCKSIZE)<0){
        fprintf(stderr,"Error reservar_bloque()%d: %s\n",errno,strerror(errno));
        return -1;
    }

    return nbloque;
}

int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
        printf("liberar_bloque() -> Ha habido un error\n");
        return -1;
    }
    escribir_bit(nbloque,0);
    SB.cantBloquesLibres++;
    if(bwrite(posSB,&SB)<0){
        printf("liberar_bloque() -> Ha habido un error\n");
        return -1;
    }

    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo inodo){
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
         printf("escribir_inodo()-> Ha habido un error\n");
         return -1;
    }
    int numBloque= ninodo/(BLOCKSIZE/INODOSIZE);
    struct inodo buffInodo[BLOCKSIZE/INODOSIZE];  
    if(bread(SB.posPrimerBloqueAI+numBloque,buffInodo)<0){
        printf("escribir_inodo()-> Ha habido un error\n");
        return -1;
    }     
    buffInodo[ninodo%(BLOCKSIZE/INODOSIZE)]=inodo;
    if(bwrite(SB.posPrimerBloqueAI+numBloque,buffInodo)<0){
        printf("escribir_inodo()-> Ha habido un error\n");
        return -1;
    }
    return 0;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    struct superbloque SB;
    if(bread(posSB, &SB)<0){
        printf("leer_inodo()-> Ha habido un error\n");
        return -1;
    }    
    int numBloque = ninodo/(BLOCKSIZE/INODOSIZE);
    struct inodo buffInodo[BLOCKSIZE/INODOSIZE];
    if(bread(SB.posPrimerBloqueAI+numBloque,buffInodo)<0){
         printf("leer_inodo()-> Ha habido un error\n");
         return -1;
    } 
    *inodo= buffInodo[ninodo%(BLOCKSIZE/INODOSIZE)];
    return 0;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
        printf("reservar_inodo() -> Ha habido un error\n");
        return -1;
    }
    int posInodoReservado = SB.posPrimerInodoLibre;
    if(SB.cantInodosLibres > 0){
        SB.posPrimerInodoLibre++;
        struct inodo InodoReservado;
        InodoReservado.tipo = tipo;
        InodoReservado.permisos = permisos;
        InodoReservado.nlinks = 1;
        InodoReservado.tamEnBytesLog = 0; 
        InodoReservado.atime=time(NULL);
        InodoReservado.mtime=time(NULL);
        InodoReservado.ctime=time(NULL);
        InodoReservado.numBloquesOcupados=0;
        for(int i=0; i<12; i++){
            InodoReservado.punterosDirectos[i]=0;
        }    
        for(int j=0; j<3; j++){
            InodoReservado.punterosIndirectos[j]=0;
        }
        escribir_inodo(posInodoReservado,InodoReservado);     
        SB.cantInodosLibres--;
        if(bwrite(posSB, &SB)<0){
            printf("reservar_inodo()-> Ha habido un error\n");
            return -1;
        }
    }else{
        printf("reservar_inodo() -> Error:No hay inodos libres\n");
    }

    return posInodoReservado;
}

int obtener_nrangoBL (struct inodo inodo, unsigned int nblogico, unsigned int *ptr){
    int nRangoBL;
    if(nblogico<DIRECTOS){
        *ptr=inodo.punterosDirectos[nblogico];
         nRangoBL=0;
    }else if(nblogico<INDIRECTOS0){
        *ptr= inodo.punterosIndirectos[0];   
         nRangoBL=1;
    }else if(nblogico<INDIRECTOS1){
        *ptr=inodo.punterosIndirectos[1];
         nRangoBL=2;
    }else if(nblogico<INDIRECTOS2){
        *ptr=inodo.punterosIndirectos[2];
         nRangoBL=3;
    }else{
        *ptr=0;
        printf("obtener_nrangoBL() -> Bloque lógico fuera de rango: %d\n", nblogico);
        nRangoBL= -1;
    }
    return nRangoBL;
}

int obtener_indice (int nblogico, int nivel_punteros){
    int indice;
    if(nblogico<DIRECTOS){
        indice = nblogico;   
    }else if(nblogico<INDIRECTOS0){
             indice = (nblogico-DIRECTOS);
    }else if(nblogico<INDIRECTOS1){            
                if(nivel_punteros==2){
                     indice = (nblogico-INDIRECTOS0)/ NPUNTEROS;            
                }else if (nivel_punteros==1){
                          indice = (nblogico-INDIRECTOS0)% NPUNTEROS;
                    }
    }else if(nblogico<INDIRECTOS2){
            if(nivel_punteros==3){
                indice = (nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);               
            }else if(nivel_punteros==2){       
                    indice = ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;               
            }else if(nivel_punteros==1){   
                        indice = ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
            }
    }
    return indice;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar){
    struct inodo inodo;
    unsigned int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice, buffer[NPUNTEROS];
    leer_inodo(ninodo,&inodo);
    ptr=0, ptr_ant=0, salvar_inodo=0;
    nRangoBL=obtener_nrangoBL(inodo, nblogico, &ptr);
    nivel_punteros=nRangoBL; 
    while(nivel_punteros>0){ 
        if(ptr==0){ 
            if(reservar==0) return -1;  
            else{ 
                salvar_inodo=1;
                ptr= reservar_bloque(); //de punteros
                inodo.numBloquesOcupados++;
                inodo.ctime=time(NULL); //fecha actual
                if(nivel_punteros==nRangoBL){
                    inodo.punterosIndirectos[nRangoBL-1]=ptr;
				  //printf("traducir_bloque_inodo() -> inodo.punterosIndirectos[%d] = %d\n", nRangoBL-1, ptr);
                  //printf("traducir_bloque_inodo() -> (reservado BF %d para punteros_nivel%d)\n",ptr,nRangoBL);
                }else{  
                    buffer[indice]=ptr;
				  //printf("traducir_bloque_inodo() -> punteros_nivel%d [%d] = %d\n",nivel_punteros+1,indice,ptr);
                  //printf("traducir_bloque_inodo() -> (reservado BF %d para punteros_nivel%d)\n",ptr,nivel_punteros);
                    bwrite(ptr_ant,buffer);
                }
            }
        }
    bread(ptr,buffer);
    indice= obtener_indice(nblogico,nivel_punteros);
    ptr_ant=ptr;
    ptr=buffer[indice];
    nivel_punteros--;
    }
    if(ptr==0){
        if(reservar==0) return -1; 
        else{
            salvar_inodo=1;
            ptr=reservar_bloque(); 
            inodo.numBloquesOcupados++;
            inodo.ctime=time(NULL);
            if(nRangoBL==0){
                inodo.punterosDirectos[nblogico]=ptr;
			  //printf("traducir_bloque_inodo() -> inodo.punterosDirectos[%d] = %d\n",nblogico,ptr); 
              //printf("traducir_bloque_inodo() -> (reservado BF %d para BL %d)\n", ptr,nblogico);               
            }else{
                buffer[indice]=ptr;
			  //printf("traducir_bloque_inodo() -> punteros_nivel%d [%d] = %d\n",nivel_punteros+1,indice,ptr); 
                //printf("traducir_bloque_inodo() -> (reservado BF %d para BL %d)\n", ptr,nblogico);               
                bwrite(ptr_ant, buffer);
            }
        }
    }
    if(salvar_inodo==1){
        escribir_inodo(ninodo, inodo);
    }

    return ptr;
}

int liberar_inodo(unsigned int ninodo){
    liberar_bloques_inodo(ninodo,0);
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    inodo.tipo='l';
    struct superbloque SB;
    if(bread(posSB,&SB)<0){
        printf("liberar_inodo() -> Ha habido un error\n");
        return -1;
    } 
    inodo.punterosDirectos[0]=SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre=ninodo;
    SB.cantInodosLibres++;
    escribir_inodo(ninodo,inodo);
    if(bwrite(posSB,&SB)<0){
        printf("liberar_inodo() -> Ha habido un error\n");
        return -1;
    }

    return ninodo;
}
int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico){   
struct inodo inodo;
unsigned int nRangoBL, nivel_punteros,indice,ptr,nblog, ultimoBL;
int bloques_punteros[3][NPUNTEROS]; 
int ptr_nivel[3]; 
int indices[3]; 
int liberados=0;

unsigned char bufAux_punteros[BLOCKSIZE];
memset(bufAux_punteros, 0, BLOCKSIZE);
leer_inodo(ninodo,&inodo);
if(inodo.tamEnBytesLog==0) return 0;
if(inodo.tamEnBytesLog% BLOCKSIZE==0){
    ultimoBL=inodo.tamEnBytesLog/BLOCKSIZE-1;
}else{
    ultimoBL=inodo.tamEnBytesLog/BLOCKSIZE;
} 
/*prueba ultimoBL=16843020;*/
printf("liberar_bloques_inodo() -> primer BL: %d, ultimoBL: %d\n", nblogico,ultimoBL);
ptr=0;
for(nblog=nblogico; nblog<=ultimoBL; nblog++){
    nRangoBL=obtener_nrangoBL(inodo,nblog,&ptr);
    if(nRangoBL<0) return -1; 
    nivel_punteros=nRangoBL;
    while(ptr>0 && nivel_punteros>0){
        if(bread(ptr, bloques_punteros[nivel_punteros-1])<0){
            printf("liberar_bloques_inodo() -> Ha habido un error\n");
            return -1;
        }
        indice=obtener_indice(nblog,nivel_punteros);
        ptr_nivel[nivel_punteros-1]=ptr;
        indices[nivel_punteros-1]=indice;
        ptr=bloques_punteros[nivel_punteros-1][indice];
        nivel_punteros--;
    }
    if(ptr>0){
        liberar_bloque(ptr);
        liberados++;
        printf("liberar_bloques_inodo()→ liberado BF %d de datos correspondiente al BL %d\n", ptr,nblog);
        if(nRangoBL==0){ 
            inodo.punterosDirectos[nblog]=0;
        }else{
            while(nivel_punteros<nRangoBL){
                indice=indices[nivel_punteros];
                bloques_punteros[nivel_punteros][indice]=0;
                ptr= ptr_nivel[nivel_punteros];
                if(memcmp(bloques_punteros[nivel_punteros], bufAux_punteros, BLOCKSIZE)==0){ 
                    liberar_bloque(ptr);
                    liberados++;
                    nivel_punteros++;
                    printf("liberar_bloques_inodo()→ liberado BF %d de punteros de nivel %d correspondiente al BL %d\n", ptr,nivel_punteros,nblog);
                    if(nivel_punteros==nRangoBL){
                        inodo.punterosIndirectos[nRangoBL-1]=0;
                    }
                }else{
                    bwrite(ptr, bloques_punteros[nivel_punteros]);
                    nivel_punteros=nRangoBL;
                }
            }
        }
    }
}
printf("liberar_bloques_inodo() -> total de bloques liberados: %d\n", liberados);
return liberados;
}

