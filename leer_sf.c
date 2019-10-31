#include "directorios.h"

void mostrarSB() { //todas las etapas
  struct superbloque SB;
  if (bread(posSB,&SB)==-1) exit(-1);
  printf("\nDATOS DEL SUPERBLOQUE\n");
  printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
  printf("posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
  printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
  printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
  printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
  printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
  printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
  printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
  printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
  printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
  printf("totBloques = %d\n", SB.totBloques);
  printf("totInodos = %d\n", SB.totInodos);
  printf("\n");
}

void mostrarTam(){
  printf("TAMAÑO SB &  TAMAÑO INODO\n");
  printf ("sizeof struct superbloque: %lu\n", sizeof(struct superbloque));
  printf ("sizeof struct inodo: %lu\n", sizeof(struct inodo));
  printf("\n");
}

void mostrarPD() {
  struct superbloque SB;
  struct inodo inodos [BLOCKSIZE/INODOSIZE];
  printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
  if (bread(posSB,&SB)==-1) exit(-1);
  for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
    if (bread(i,inodos)==-1) exit(-1);
    for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++){
      // printf("%d ",inodos[j].punterosDirectos[0]);
    }
  }
  printf("\n");
}

void pruebaRL() {
  int  nbloque;
  struct superbloque SB;
  printf("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
  if (bread(posSB,&SB)==-1) exit(-1);
  nbloque = reservar_bloque();
  printf("Se ha reservado el bloque físico nº %d que era el 1º libre indicado por el MB\n",nbloque );
  if (bread(posSB,&SB)==-1) exit(-1);
  printf("cantBloquesLibres: %d\n", SB.cantBloquesLibres);
  liberar_bloque(nbloque);
  if (bread(posSB,&SB)==-1) exit(-1);
  printf("cantBloquesLibres: %d\n", SB.cantBloquesLibres);
  printf("\n");
}

void mostrarMB(){
  struct superbloque SB;
  if (bread(posSB,&SB)==-1) exit(-1);
  printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
  printf("valor del bit correspondiente a posSB (o sea al BF nº %d)  = %d\n",posSB,leer_bit(posSB));
  printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %d) = %d\n",SB.posPrimerBloqueMB,leer_bit(SB.posPrimerBloqueMB));
  printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %d) = %d\n",SB.posUltimoBloqueMB,leer_bit(SB.posUltimoBloqueMB));
  printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %d) = %d\n",SB.posPrimerBloqueAI,leer_bit(SB.posPrimerBloqueAI));
  printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %d) = %d\n",SB.posUltimoBloqueAI,leer_bit(SB.posUltimoBloqueAI));;
  printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %d) = %d\n",SB.posPrimerBloqueDatos,leer_bit(SB.posPrimerBloqueDatos));
  printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %d) = %d\n",SB.posUltimoBloqueDatos,leer_bit(SB.posUltimoBloqueDatos));
  printf("\n");
}

//modificado para leer cualquier inodo
void leerDRaiz(int posinodo){
  struct superbloque SB;
  printf("DATOS DEL INODO RESERVADO\n");
  if (bread(posSB,&SB)==-1) exit(-1);
  struct tm *ts;
  char atime[80];
  char mtime[80];
  char ctime[80];
  struct inodo inodos;
  leer_inodo(posinodo, &inodos);
  printf("tipo: %c\n", inodos.tipo);
  printf("permisos: %d\n", inodos.permisos);
  ts = localtime(&inodos.atime);
  strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S\n", ts);
  ts = localtime(&inodos.mtime);
  strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S\n", ts);
  ts = localtime(&inodos.ctime);
  strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S\n", ts);
  printf("ATIME: %sCTIME: %sMTIME: %s", atime,ctime,mtime);
  printf("nlink: %d\n", inodos.nlinks);
  printf("tamEnBytesLog: %d\n", inodos.tamEnBytesLog);
  printf("numBloquesOcupados: %d\n", inodos.numBloquesOcupados);
  printf("\n");
  printf("SB.posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
  printf("\n");
}

int tradBloques(){
  printf("INODO 1. TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 16843019\n");
  int  ninodo;
  unsigned char c = 'f';
  unsigned char u = 6;
  ninodo = reservar_inodo(c,u);
  traducir_bloque_inodo(ninodo,8,'1');
  traducir_bloque_inodo(ninodo,204,'1');
  traducir_bloque_inodo(ninodo,30004,'1');
  traducir_bloque_inodo(ninodo,400004,'1');
  traducir_bloque_inodo(ninodo,16843019,'1');
  printf("\n");
  return ninodo;
}
void liberarInodo(int ninodo){
	printf("LIBERAMOS EL INODO RESERVADO EN LA ETAPA ANTERIOR Y TODOS SUS BLOQUES\n");
  	liberar_inodo(ninodo);
  	printf("\n");

}

int main(int argc, char **argv){
  //int ninodo;
  if (argc!=2) {
    fprintf(stderr, "Sintaxis: leer_sf <nombre_dispositivo>\n");
    exit(-1);
  }
  bmount(argv[1]);
  //Muestra los datos del Superbloque
  mostrarSB();
  //Tamaño SB y inodo
  //mostrarTam();
  //Array de inodos punteros directos
  //mostrarPD();
  //Reservar bloque y liberar bloque
  //pruebaRL();
  //Mostrar mapa de bits con bloques de metadatos ocupados
  //mostrarMB();
  //Traducir bloque
 // ninodo = tradBloques();
   //Leer nodo
  //leerDRaiz(ninodo);
  //liberarInodo(ninodo);
  struct superbloque SB;
  if (bread(posSB,&SB)==-1) exit(-1);
  fprintf(stderr,"SB.posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
  bumount(argv[1]);
}
