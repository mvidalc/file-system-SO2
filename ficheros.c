
#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned nbytes)
{
    struct inodo inodo;

    unsigned char buf_bloque[BLOCKSIZE];
    int Blog = offset / BLOCKSIZE;
    int ultBlog = (offset + nbytes - 1) / BLOCKSIZE;
    int desp1 = offset % BLOCKSIZE;
    int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    int bfisico;
    int bytesE;
    
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 2) == 2)
    {
        if (Blog == ultBlog)
        {
            mi_waitSem();
            bfisico = traducir_bloque_inodo(ninodo, Blog, 1);
            mi_signalSem();
            bread(bfisico, buf_bloque);
            memcpy(buf_bloque + desp1, buf_original, nbytes);
            bwrite(bfisico, buf_bloque);
            bytesE = nbytes;
         
        }
        else
        {   mi_waitSem();
            bfisico = traducir_bloque_inodo(ninodo, Blog, 1);
            mi_signalSem();
            bread(bfisico, buf_bloque);

            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            bwrite(bfisico, buf_bloque);
            bytesE = BLOCKSIZE - desp1;
       
            for (int i = Blog+1; i < ultBlog; i++)
            {
                mi_waitSem();
                bfisico = traducir_bloque_inodo(ninodo, i, 1);
                mi_signalSem();
                bread(bfisico, buf_bloque);

                memcpy(buf_bloque, buf_original + (BLOCKSIZE - desp1) + (i - Blog - 1) * BLOCKSIZE, BLOCKSIZE);
                bwrite(bfisico, buf_original + (BLOCKSIZE - desp1) + (i - Blog - 1) * BLOCKSIZE);
                bytesE = bytesE + BLOCKSIZE;
              
            }
            mi_waitSem();
            bfisico = traducir_bloque_inodo(ninodo, ultBlog, 1);
            mi_signalSem();
            bread(bfisico, buf_bloque);

            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            bwrite(bfisico, buf_bloque);
            bytesE = bytesE + desp2 + 1;
        
        }
        mi_waitSem();
        leer_inodo(ninodo, &inodo);
        if (inodo.tamEnBytesLog < offset + bytesE)
        {
            inodo.tamEnBytesLog = offset + bytesE;
            inodo.ctime = time(NULL);
        }
        inodo.mtime = time(NULL);
        if(escribir_inodo(ninodo, inodo)==-1){
            perror("Error\n");
             mi_signalSem();
        }
          mi_signalSem();
        }
      
    else
    {
        fprintf(stderr, "No se dispone de permisos de escritura\n");
        bytesE=0;
        return -1;
    }
    return bytesE;
}
 int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    int leidos = 0;
   
    int bfisico;
    unsigned char buf_bloque[BLOCKSIZE];
    mi_waitSem();
    if(leer_inodo(ninodo, &inodo)==-1){
        mi_signalSem();
        return -1;
    }   
    inodo.atime = time(NULL);
    if(escribir_inodo(ninodo, inodo)==-1){
        mi_signalSem();
        return -1;
    } 
    mi_signalSem();
    if ((inodo.permisos & 4) == 4){
        if (offset >= inodo.tamEnBytesLog){
            leidos = 0;
            return leidos;
        } 
 		if ((offset + nbytes) >= inodo.tamEnBytesLog){
            nbytes = inodo.tamEnBytesLog - offset;
            //fprintf(stderr,"[mi_read_f] nbytes ultimo: %d\n", nbytes);
		}
        int Blog = offset / BLOCKSIZE;
        int ultBlog = (offset + nbytes - 1) /BLOCKSIZE;
        int desp1 = offset % BLOCKSIZE;
        int desp2 = (offset + nbytes -1) % BLOCKSIZE;
		if(Blog == ultBlog){
            bfisico = traducir_bloque_inodo(ninodo,Blog,0);
            if(bfisico != -1){
                if(bread(bfisico,buf_bloque)==-1){
                    perror("Error\n");
                    return -1;
                }
                memcpy(buf_original,buf_bloque+desp1,nbytes);
            }  
            leidos=nbytes;
		}else{
            bfisico = traducir_bloque_inodo(ninodo, Blog,0);
            if(bfisico != -1){
                if(bread(bfisico,buf_bloque)==-1){
                    perror("Error\n");
                    return -1;
                }
                memcpy(buf_original,buf_bloque+desp1,BLOCKSIZE-desp1);
            } 
            leidos=BLOCKSIZE - desp1;
            int i;
            for(i = Blog+1; i<ultBlog; i++){
                bfisico = traducir_bloque_inodo(ninodo,i,0);
                if(bfisico != -1){
                    if(bread(bfisico,buf_bloque)==-1){
                        perror("Error\n");
                        return -1;
                    }
                    memcpy(buf_original + (BLOCKSIZE - desp1) + (i - Blog - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
                }
                leidos = leidos + BLOCKSIZE;
            }
            
            bfisico = traducir_bloque_inodo(ninodo,ultBlog,0);
            if(bfisico != -1){
                    if(bread(bfisico,buf_bloque)==-1){
                        perror("Error\n");
                        return -1;
                    }
                    memcpy(buf_original+(nbytes-desp2-1),buf_bloque,desp2+1);
            }
            leidos = leidos + desp2 + 1;
        }          
    }else{     
        fprintf(stderr,"No hay permisos de lectura\n");
        return-1;
    }
    return leidos;
} 

    int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
    {
        struct inodo inodo;
        leer_inodo(ninodo, &inodo);
        p_stat->tipo = inodo.tipo;
        p_stat->permisos = inodo.permisos;
        p_stat->atime = inodo.atime;
        p_stat->ctime = inodo.ctime;
        p_stat->mtime = inodo.mtime;
        p_stat->nlinks = inodo.nlinks;
        p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
        p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
        if (escribir_inodo(ninodo, inodo) == -1)
        {
            perror("Error\n");
            return -1;
        }
        printf("nº de inodo: %d\n",ninodo);
        return 0;
    }

    int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
    {
        struct inodo inodo;
        mi_waitSem();
        if(leer_inodo(ninodo, &inodo)==-1){
             mi_signalSem();
             return -1;
        };
        inodo.permisos = permisos;
        inodo.ctime = time(NULL);
        if(escribir_inodo(ninodo, inodo)==-1){
             mi_signalSem();
             return -1;
        }
         mi_signalSem();
        return 0;
    }
    int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
    {
        int liberados = 0;
        int nblogico;
        struct inodo inodo;
        if (leer_inodo(ninodo, &inodo) == -1)
        {
            perror("Error\n");
            return -1;
        }
        if ((inodo.permisos & 2) != 2)
        {
            fprintf(stderr, "No hay permisos de escritura\n");
            return -1;
        }

        if (nbytes < inodo.tamEnBytesLog)
        {
            if (nbytes % BLOCKSIZE == 0)
            {
                nblogico = nbytes / BLOCKSIZE;
            }
            else
            {
                nblogico = nbytes / BLOCKSIZE + 1;
            }
            if (nblogico >= INDIRECTOS2)
            {
                fprintf(stderr, "Error\n");
                return -1;
            }
            liberados = liberar_bloques_inodo(ninodo, nblogico);
            if (liberados == -1)
            {
                fprintf(stderr, "Error,saS\n");
                return -1;
            }
            inodo.mtime = time(NULL);
            inodo.ctime = time(NULL);
            inodo.tamEnBytesLog = nbytes;
            inodo.numBloquesOcupados -= liberados;
            if (escribir_inodo(ninodo, inodo) == -1)
            {
                perror("Error\n");
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "No se puede truncar más allá del EOF: %d\n", inodo.tamEnBytesLog);
        }

       /* struct superbloque SB;
        printf("DATOS DEL INODO %d\n", ninodo);
        if (bread(posSB, &SB) == -1)
            exit(-1);
        struct tm *ts;
        char atime[80];
        char mtime[80];
        char ctime[80];
        struct inodo inodos;
        leer_inodo(ninodo, &inodos);
        printf("tipo: %c\n", inodos.tipo);
        printf("permisos: %d\n", inodos.permisos);
        ts = localtime(&inodos.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S\n", ts);
        ts = localtime(&inodos.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S\n", ts);
        ts = localtime(&inodos.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S\n", ts);
        printf("ATIME: %sCTIME: %sMTIME: %s", atime, ctime, mtime);
        printf("nlink: %d\n", inodos.nlinks);
        printf("tamEnBytesLog: %d\n", inodos.tamEnBytesLog);
        printf("numBloquesOcupados: %d\n", inodos.numBloquesOcupados);
        printf("\n");*/
        return liberados;
    }