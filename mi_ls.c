#include "directorios.h"

int main(int argc, char **argv) {
    char buffer[BLOCKSIZE];
    int nentradas;
    if (argc != 3) {
        fprintf(stderr, "Syntax err or: ./mi_ls <disco> </ruta_directorio>\n");
        return -1;
    }

    bmount(argv[1]);
    memset(buffer,'\0', BLOCKSIZE);
    nentradas = mi_dir(argv[2], buffer);
    printf("NENTRADAS: %d\n",nentradas);
    if (nentradas <0){
        printf("ERROR MI_LS");
        return -1;
    }
    bumount();
}