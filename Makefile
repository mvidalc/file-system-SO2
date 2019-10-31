CC=gcc #compilador
CFLAGS=-c -g -Wall -std=gnu99 #flags para el compilador
LDFLAGS=-pthread #flags para enlazador

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c ficheros.c leer_sf.c directorios.c mi_ls.c mi_mkdir.c mi_stat.c mi_chmod.c mi_cat.c mi_escribir.c mi_rm.c mi_link.c semaforo_mutex_posix.c simulacion.c verificacion.c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o semaforo_mutex_posix.o
INCLUDES=bloques.h ficheros_basico.h ficheros.h directorios.h semaforo_mutex_posix.h simulacion.h verificacion.h
PROGRAMS= mi_mkfs leer_sf mi_ls mi_mkdir mi_stat mi_chmod mi_cat mi_escribir mi_rm mi_link simulacion verificacion
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o $(PROGRAMS)	
