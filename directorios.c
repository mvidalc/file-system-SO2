#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    char barra = '/';
    char *ret;

    int type; //1 en caso de ser fichero y 0 en caso de ser directorio.
    //Metemos en inicial el "dir1" sin la barra
    if (camino[0] != barra)
    {
        printf("El camino debe empezar con %c", barra);
        return -1;
    }
    camino++;
    ret = strchr(camino, barra);

    if (ret != NULL)
    {
        unsigned int longIni = (ret - camino);
        strncpy(inicial, camino, longIni);
        inicial[longIni] = '\0';
        camino = ret;
        strcpy(final, camino);
        *tipo = 'd';
        type = 0;
        //Indicamos que se trata de un directorio al haber mas barras aparte de la primera
    }
    else
    {
        strcpy(inicial, camino);
        strcpy(final, "\0");
        *tipo = 'f';
        type = 1;

        //Indicamos que se trata de un fichero;
    }

    return type;
}
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    struct inodo inodo;
    struct entrada ent;
    char inicial[sizeof(ent.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;

    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = 0; //La raíz siempre estara asociada al inodo 0.
        *p_entrada = 0;
        return 0;
    }

    // Limpiamos inicial y final
    memset(inicial, 0, sizeof(ent.nombre));
    memset(final, 0, sizeof(strlen(camino_parcial)));
    int extr_cam = extraer_camino(camino_parcial, inicial, final, &tipo);

    if (extr_cam == -1)
    {
        return -1;
    }

    //fprintf(stderr, "[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n",inicial,final,reservar);

    if (leer_inodo(*p_inodo_dir, &inodo))
    {
        fprintf(stderr, "buscar_entrada --> Error en leer_inodo");
        return -1;
    }

    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "[buscar_entrada()→ El inodo %d no tiene permisos de lectura].\n", *p_inodo_dir);
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        return -1;
    }

    //inicializar el nombre de la entrada
    memset(ent.nombre, 0, sizeof(ent.nombre));

    //calcular el nº de entradas del inodo, o sea numentradas
    int numentradas = inodo.tamEnBytesLog / sizeof(struct entrada);
    int nentrada = 0; //nº de entrada inicial
    if (numentradas > 0)
    {
        //leer entrada
        mi_read_f(*p_inodo_dir, &ent, nentrada * sizeof(ent), sizeof(ent));
        while ((nentrada < numentradas) && (strcmp(inicial, ent.nombre) != 0))
        {
            nentrada++;
            mi_read_f(*p_inodo_dir, &ent, nentrada * sizeof(ent), sizeof(ent));
        }
    }

    if (nentrada == numentradas)
    {
        //fprintf(stderr, "Entrada no encontrada\n");
        switch (reservar)
        {
        case 0:
            fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
            return -1;
        case 1:

            if (inodo.tipo == 'f')
            {
                printf("buscar_entrada --> No se puede crear una entrada en un fichero.\n");
                return -1;
            }
            if ((inodo.permisos & 2) != 2)
            {
                printf("buscar_entrada --> Error: permiso de escritura denegado.\n");
                return -1;
            }
            else
            {
                strcpy(ent.nombre, inicial);
                if (tipo == 'd')
                {
                    if (strcmp(final, "/") == 0)
                    {
                        ent.ninodo = reservar_inodo('d', permisos);
                        //fprintf(stderr, "[buscar_entrada()→ entrada.nombre: %s, entrada.ninodo: %d]\n",ent.nombre,ent.ninodo);
                        //fprintf(stderr, "[buscar_entrada()→ reservado inodo %d tipo d con permisos %d]\n",ent.ninodo,permisos);
                    }
                    else
                    {
                        printf("buscar_entrada -->Error: No existe directorio intermedio.\n");
                        return -1;
                    }
                }
                else
                {

                    ent.ninodo = reservar_inodo('f', permisos);
                    //fprintf(stderr, "[buscar_entrada()→ entrada.nombre: %s, entrada.ninodo: %d]\n",ent.nombre,ent.ninodo);
                    //fprintf(stderr, "[buscar_entrada()→ reservado inodo %d tipo f con permisos %d]\n",ent.ninodo,permisos);
                }

                if (mi_write_f(*p_inodo_dir, &ent, nentrada * sizeof(ent), sizeof(ent)) == -1)
                {
                    if (ent.ninodo != -1)
                    {
                        liberar_inodo(ent.ninodo);
                    }
                    printf("buscar_entrada --> Error: el inodo no tiene permisos de escritura.\n");
                    return -1;
                }
            }
        }
    }
    if (strcmp(final, "/") == 0 || strcmp(final, "") == 0)
    {
        if ((nentrada < numentradas) && (reservar == 1))
        {
            fprintf(stderr, "buscar_entrada --> Error: La entrada ya existe.\n");
            return -1;
        }
        *p_inodo = ent.ninodo;
        *p_entrada = nentrada;
        return 0;
    }
    else
    {
        *p_inodo_dir = ent.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }

    return 0;
}

int mi_creat(const char *camino, unsigned char permisos)
{
    mi_waitSem();
    struct superbloque SB;
    if (bread(posSB, &SB) == -1)
    {
        perror("ERROR: ");
        return -1;
    }
    unsigned int pe = 0;
    unsigned int pi = 0;
    unsigned int pid = SB.posInodoRaiz;
    int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 1, permisos);
    if (bentrada < 0)
    {
        switch (bentrada)
        {
        case -1:
            printf("\"%s\" Camino inválido \n", camino);
            break;
        case -2:
            printf("Permisos de lectura necesarios \n");
            break;
        case -4:
            printf("Entrada inexistente \n");
            break;
        case -5:
            printf("Error al reservar inodo\n");
            break;
        case -6:
            printf("No existe directorio intermedio a \"%s\"\n", camino);
            break;
        case -7:
            printf("Error al liberar inodo \n");
            break;
        case -8:
            printf("Error de escritura de \"%s\"\n", camino);
            break;
        case -9:
            printf("Ya existe \"%s\"\n", camino);
            break;
        }
        puts("Error en mi_creat");
        return -1;
    }
    mi_signalSem();
    return 0;
}

int mi_dir(const char *camino, char *buffer)
{
    unsigned int pi = 0, pe = 0;
    struct superbloque SB;
    if (bread(posSB, &SB) == -1)
    {
        perror("ERROR: ");
        return -1;
    }
    unsigned int pid = SB.posInodoRaiz;
    int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
    if (bentrada < 0)
    {
        switch (bentrada)
        {
        case -1:
            printf("\"%s\" Camino inválido \n", camino);
            break;
        case -2:
            printf("Permisos de lectura necesarios \n");
            break;
        case -4:
            printf("Entrada inexistente \n");
            break;
        case -5:
            printf("Error al reservar inodo\n");
            break;
        case -6:
            printf("No existe directorio intermedio a \"%s\"\n", camino);
            break;
        case -7:
            printf("Error al liberar inodo \n");
            break;
        case -8:
            printf("Error de escritura de \"%s\"\n", camino);
            break;
        case -9:
            printf("Ya existe \"%s\"\n", camino);
            break;
        }
        puts("Error en mi_dir");
        return -1;
    }

    struct inodo in;
    leer_inodo(pi, &in);
    struct entrada entr;
    if (in.tipo != 'd' && in.permisos & 4)
    {
        return -1;
    }
    int numentradas = in.tamEnBytesLog / sizeof(struct entrada);
    int nentrada = 0;
    while (nentrada < numentradas)
    {
        if (mi_read_f(pi, &entr, nentrada * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return -2;
        }
        leer_inodo(entr.ninodo, &in);
        if (in.tipo == 'd')
        {
            strcat(buffer, "d");
        }
        else
        {
            strcat(buffer, "f");
        }

        strcat(buffer, "\t");

        if (in.permisos & 4)
        {
            strcat(buffer, "r");
        }
        else
        {
            strcat(buffer, "-");
        }
        if (in.permisos & 2)
        {
            strcat(buffer, "w");
        }
        else
        {
            strcat(buffer, "-");
        }
        if (in.permisos & 1)
        {
            strcat(buffer, "x");
        }
        else
        {
            strcat(buffer, "-");
        }

        strcat(buffer, "\t");

        struct tm *tm;
        char tmp[100];
        tm = localtime(&in.mtime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        strcat(buffer, tmp);
        strcat(buffer, "\t");
        char buff[100];
        sprintf(buff, "%d", in.tamEnBytesLog);
        strcat(buffer, buff);
        strcat(buffer, "\t");
        strcat(buffer, entr.nombre);
        strcat(buffer, "\n");
        nentrada++;
    }
    printf("Total: %d\n", nentrada);
    printf("Tipo	Permisos    mTime 			Tamaño	    Nombre\n");
    printf("------------------------------------------------------------------\n");
    printf("%s\n", buffer);
    printf("Numentradas: %d\n", numentradas);
    return numentradas;
}

int mi_chmod(const char *camino, unsigned char permisos)
{
    unsigned int pid = 0, pi = 0, pe = 0;
    int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, permisos);
    if (bentrada < 0)
    {
        switch (bentrada)
        {
        case -1:
            printf("\"%s\" Camino inválido \n", camino);
            break;
        case -2:
            printf("Permisos de lectura necesarios \n");
            break;
        case -4:
            printf("Entrada inexistente \n");
            break;
        case -5:
            printf("Error al reservar inodo\n");
            break;
        case -6:
            printf("No existe directorio intermedio a \"%s\"\n", camino);
            break;
        case -7:
            printf("Error al liberar inodo \n");
            break;
        case -8:
            printf("Error de escritura de \"%s\"\n", camino);
            break;
        case -9:
            printf("Ya existe \"%s\"\n", camino);
            break;
        }
        puts("Error en mi_chmod");
        return -1;
    }
    else
    {
        if (mi_chmod_f(pi, permisos) < 0)
        {
            return -2;
        }
    }
    return 0;
}

int mi_stat(const char *camino, struct STAT *p_stat)
{
    unsigned int pid = 0, pi = 0, pe = 0;
    int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
    if (bentrada < 0)
    {
        switch (bentrada)
        {
        case -1:
            printf("\"%s\" Camino inválido \n", camino);
            break;
        case -2:
            printf("Permisos de lectura necesarios \n");
            break;
        case -4:
            printf("Entrada inexistente \n");
            break;
        case -5:
            printf("Error al reservar inodo\n");
            break;
        case -6:
            printf("No existe directorio intermedio a \"%s\"\n", camino);
            break;
        case -7:
            printf("Error al liberar inodo \n");
            break;
        case -8:
            printf("Error de escritura de \"%s\"\n", camino);
            break;
        case -9:
            printf("Ya existe \"%s\"\n", camino);
            break;
        }
        puts("Error en mi_stat");
        return -1;
    }
    else
    {
        if (mi_stat_f(pi, p_stat) < 0)
        {
            return -2;
        }
    }
    return 0;
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int pid;
    unsigned int pin = 0;
    unsigned int pe = 0;
    unsigned int BytesLeidos;
    struct superbloque SB;
    int bentrada;
    bread(posSB, &SB);
    pid = SB.posInodoRaiz;

    /*if(strcmp(camino, UltimaEntradaLectura.camino) == 0){
        pin = UltimaEntradaLectura.p_inodo;
        printf("[mi_read() → Utilizamos la caché de lectura en vez de llamar a buscar_entrada()]\n");

    }else{
    bentrada = buscar_entrada(camino, &pid, &pin, &pe, 0, 6);
    UltimaEntradaLectura.p_inodo=pin;
     printf("[mi_read() → Actualizamos la caché de lectura]\n");

    }*/
    bentrada = buscar_entrada(camino, &pid, &pin, &pe, 0, 6);
    if (bentrada < 0)
    {
        switch (bentrada)
        {
        case -1:
            printf("\"%s\" Camino inválido \n", camino);
            break;
        case -2:
            printf("Permisos de lectura necesarios \n");
            break;
        case -4:
            printf("Entrada inexistente \n");
            break;
        case -5:
            printf("Error al reservar inodo\n");
            break;
        case -6:
            printf("No existe directorio intermedio a \"%s\"\n", camino);
            break;
        case -7:
            printf("Error al liberar inodo \n");
            break;
        case -8:
            printf("Error de escritura de \"%s\"\n", camino);
            break;
        case -9:
            printf("Ya existe \"%s\"\n", camino);
            break;
        }
        puts("Error en mi_read");
        return -1;
    }
    else
    {
        if ((BytesLeidos = mi_read_f(pin, buf, offset, nbytes)) < 0)
        {
            return -2;
        }
    }
    return BytesLeidos;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int pid;
    unsigned int pin = 0;
    unsigned int pe = 0;
    unsigned int BytesEscritos;
    struct superbloque SB;
    unsigned int bentrada;
    bread(posSB, &SB);
    pid = SB.posInodoRaiz;
    /*if(strcmp(camino, UltimaEntradaEscritura.camino) == 0){
        pin = UltimaEntradaEscritura.p_inodo;
        printf("[mi_write() → Utilizamos la caché de escritura en vez de llamar a buscar_entrada()]\n");

    }else{
    bentrada = buscar_entrada(camino, &pid, &pin, &pe, 0, 6);
    UltimaEntradaEscritura.p_inodo=pin;
     printf("[mi_write() → Actualizamos la caché de escritura]\n");

    }*/
    bentrada = buscar_entrada(camino, &pid, &pin, &pe, 0, 6);

    if (bentrada < 0)
    {
        switch (bentrada)
        {
        case -1:
            printf("\"%s\" Camino inválido \n", camino);
            break;
        case -2:
            printf("Permisos de lectura necesarios \n");
            break;
        case -4:
            printf("Entrada inexistente \n");
            break;
        case -5:
            printf("Error al reservar inodo\n");
            break;
        case -6:
            printf("No existe directorio intermedio a \"%s\"\n", camino);
            break;
        case -7:
            printf("Error al liberar inodo \n");
            break;
        case -8:
            printf("Error de escritura de \"%s\"\n", camino);
            break;
        case -9:
            printf("Ya existe \"%s\"\n", camino);
            break;
        }
        puts("Error en mi_write");
        return -1;
    }
    struct inodo in;
    leer_inodo(pin, &in);
    if (in.tipo != 'f')
    {
        puts("ERROR lectura de directorio");
        return -3;
    }
    if ((BytesEscritos = mi_write_f(pin, buf, offset, nbytes)) < 0)
    {
        return -2;
    }
    return BytesEscritos;
}

int mi_link(const char *camino1, const char *camino2)
{
    struct superbloque SB;
    if (bread(posSB, &SB) == -1)
    {
        perror("Error: ");
        return -1;
    }
    unsigned int p_id1 = SB.posInodoRaiz;
    unsigned int p_id2 = SB.posInodoRaiz;

    unsigned int p_i1 = 0;
    unsigned int p_i2 = 0;

    unsigned int p_entrada1 = 0;
    unsigned int p_entrada2 = 0;

    char reservar = 0;
    char permisos = 0;

    struct inodo inodo1;
    struct inodo inodo2;
    struct entrada entr;
    mi_waitSem();
    int error = buscar_entrada(camino1, &p_id1, &p_i1, &p_entrada1, reservar, permisos);
    if (error == -1)
    {
        fprintf(stderr, "Error en mi_link");
        mi_signalSem();
        return -1;
    }

    if (leer_inodo(p_i1, &inodo1) == -1)
    {
        mi_signalSem();
        return -1;
    }
    if (inodo1.tipo != 'f')
    {
        fprintf(stderr, "Error en mi_link => l'inode no és del tipus correcte");
        mi_signalSem();
        return -1;
    }

    reservar = 1;
    permisos = 6;

    error = buscar_entrada(camino2, &p_id2, &p_i2, &p_entrada2, reservar, permisos);
    if (error == -1)
    {
        fprintf(stderr, "Error en mi_link => error quan es crida a buscar entrada 2");
        mi_signalSem();
        return -1;
    }

    if (leer_inodo(p_i2, &inodo2) == -1)
    {
        mi_signalSem();
        return -1;
    }
    if (inodo2.tipo != 'f')
    {
        fprintf(stderr, "Error en mi_link => l'inode no és del tipus correcte");
        mi_signalSem();
        return -1;
    }

    if (mi_read_f(p_id2, &entr, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) == -1)
    {
        fprintf(stderr, "Error en mi_link => mi_read_f");
        mi_signalSem();
        return -1;
    }

    entr.ninodo = p_i1;

    if (mi_write_f(p_id2, &entr, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) == -1)
    {
        fprintf(stderr, "Error en mi_link => mi_write_f");
        mi_signalSem();
        return -1;
    }

    if (liberar_inodo(p_i2) == -1)
    {
        fprintf(stderr, "Error en mi_link => liberar_inodo");
        mi_signalSem();
        return -1;
    }

    inodo1.nlinks++;
    inodo1.ctime = time(NULL);

    if (escribir_inodo(p_i1, inodo1) == -1)
    {
        fprintf(stderr, "Error en mi_link => escribir_inodo");
        mi_signalSem();
        return -1;
    }
    mi_signalSem();
    return 0;
}

int mi_unlink(const char *camino)
{
    struct superbloque SB;
    
    if (bread(posSB, &SB) == -1)
    {
        perror("Error: ");
            
        return -1;
    }
    unsigned int p_id = SB.posInodoRaiz;
    unsigned int p_in = 0;
    unsigned int p_entrada = 0;

    struct inodo inodo;
    struct inodo inodo_dir;
    struct entrada entr;

    char reservar = 0;
    char permisos = 0;
    int numentradas = 0;
    mi_waitSem();
    int error = buscar_entrada(camino, &p_id, &p_in, &p_entrada, reservar, permisos);
    if (error == -1)
    {
         mi_signalSem();
        return -1;
    }

    if (leer_inodo(p_in, &inodo) == -1){
         mi_signalSem();
         return -1;
    }
        
    if ((inodo.tipo == 'd') && (inodo.tamEnBytesLog > 0))
    {
        fprintf(stderr, "Error el directorio no esta vacío\n");
         mi_signalSem();
        return -1;
    }
    if (leer_inodo(p_id, &inodo_dir) == -1){
         mi_signalSem();
        return -1;
    }
        

    numentradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);

    if (p_entrada != numentradas - 1)
    {
        mi_read_f(p_id, &entr, inodo_dir.tamEnBytesLog - sizeof(struct entrada), sizeof(struct entrada));
        mi_write_f(p_id, &entr, (p_entrada) * sizeof(struct entrada), sizeof(struct entrada));
    }

    mi_truncar_f(p_id, inodo_dir.tamEnBytesLog - sizeof(struct entrada));

    if (leer_inodo(p_in, &inodo) == -1){
         mi_signalSem();
          return -1;
    }
       

    if (inodo.nlinks == 1)
    {
        if (liberar_inodo(p_in) == -1)
        {
            fprintf(stderr, "Error en mi_unlink => liberar_inodo\n");
             mi_signalSem();
            return -1;
        }
    }
    else
    {
        inodo.nlinks--;
        inodo.ctime = time(NULL);
        if (escribir_inodo(p_in, inodo) == -1)
        {
            fprintf(stderr, "Error en mi_unlink => escribir_inodo\n");
             mi_signalSem();
            return -1;
        }
    }
     mi_signalSem();
    return 0;
}
