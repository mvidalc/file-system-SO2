#include <string.h>
#include "directorios.h"

int main(int argc, char **argv)
{
  unsigned char buf[BLOCKSIZE];
  bmount(argv[1]);
  memset(buf, 0, BLOCKSIZE);
  int nbloques = atoi(argv[2]);
  int ninodos = nbloques / 4;
  for (int i = 0; i < nbloques; i++)
  {
    bwrite(i, buf);
  }
  initSB(nbloques, ninodos);
  initMB();
  initAI();
  reservar_inodo('d', 7);
  bumount();

  return 0;
}
