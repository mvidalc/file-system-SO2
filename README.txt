******************* Miembros del grupo ********************

Abdulaye Diakhate Ariza		-	

Miguel Vidal Coll 			-	43225456S	

Sergi Vivo Valls			-	45695276A

***********************************************************

La practica trata sobre la implementación de un sistema de 
ficheros basado en inodos, implementado en el lenguaje de 
programacion C.

En conjunto la práctica consta de 19 ficheros en formato C 
con sus respectivos ficheros de declaraciones, además de un 
makefile para crear los objetos ejecutables.

A continuación describiremos brevemente el contenido de cada 
fichero y su utilidad dentro de la práctica, explicando las 
lastricciones y la sintaxis específica en caso de ser 
necesaria.

*	bloques.c

Permite a otros métodos interactuar con los bloques, con 
operaciones para montar, desmontar, leer o escribir los 
bloques. Además, los métodos bmount i bumount están 
implementados usando semáforos para evitar problemas de 
concurrencia.

*	directorios.c

Permite al usuario interactuar con los directorios, 
mediante operaciones para buscar la entrada, extraer el 
camino introducido por el usuario, etc. Este fichero cuenta 
con la implementación de nuestra version de las operaciones 
de lectura, escritura, direccionamiento, etc.

*	ficheros.c

Permite al usuario realizar operaciones como escribir, leer 
truncar o cambiar los permisos que se tienen en un fichero.

*	ficheros_basico.c

Fichero en el cual se especifican la estructura del 
superbloque. También contiene instrucciones relacionadas 
tanto con el superbloque como los inodos: operaciones de 
lectura, escritura, reserva, traducción, etc.

*	leer.c

Permite al usuario leer un fichero mediante la sintaxis:
	leer <nombre_dispositivo> <nºinodo>
Además, muestra por pantalla el numero de bytes leídos y 
el tamaño en bytes lógico.

*	leer_sf.c

Muestra la estructura del superbloque, además de su tamaño y
el mapa de bits. 

*	mi_cat.c, mi_chmod.c, mi_escribir.c, mi_escribir_varios.c, 
	mi_link.c, mi_ls.c, mi_mkdir.c, mi_mkfs.c, mi_rm.c, 
	mi_stat.c

Estos ficheros contienen nuestras implementaciones de las 
funciones elementales de mismo nombre adaptadas a los 
requisitos de nuestro sistema de ficheros. Cada uno tiene 
una sintaxis específica.

 *	semaforo_mutex_posix.c

Implementación de los semáforos POSIX, con las funciones 
delete, signal y wait.

 *	simulacion.c

Fichero que ejecuta la simulación de 100 procesos realizando 
50 escrituras cada uno en el disco

 *	verificacion.c

Fichero que comprueba que las escrituras realizadas durante 
la simulación se hayan llevado a cabo correctamente. Para 
ello, muestra información sobre cada proceso: la primera 
escritura, la última, la de menor posición y la de mayor 
posición, además de mostrar el tamaño y la fecha de cada una 
de estas operaciones.

Para finalizar, cabe mencionar que no se ha realizado ninguna 
de las mejoras propuestas debido al elevado numero de problemas 
que hemos obtenido intentando implementarlas.