/******************************************************************************
 * Fichero: Intercomunicadores4.c
 *
 * Compilar con:    mpicc Intercomunicadores4.c -o inter4
 * Ejecutar con:    mpirun -np 1 inter4

 * Un proceso maestro genera de manera dinámica M procesos, siendo M un número par. 
 * A su vez, este proceso maestro generará M/2 números aleatorios y se los enviará 
 * a los procesos con rango par, que lo multiplicarán por otro número aleatorio. 
 * Luego el maestro creará otros M/2 números aleatorios y se los enviará a los 
 * procesos de rango impar, que también lo multiplicarán por un número aleatorio. 
 * Finalmente el proceso maestro deberá conocer cuál es el mayor valor resultante 
 * de las multiplicaciones en cada uno de los procesos de rango par, y el menor valor 
 * de entre los procesos de rango impar.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
 
#define PROCESOS 6
/******************************************************************************
 * Programa principal
 *****************************************************************************/
int main(int argc, char *argv[]) {
    // Declaración de variables
    int procesos;			// Número de procesos creados de forma estática, que será 1
    int miRango;                        
    MPI_Comm intercom, intercom2;          // Intercomunicador e intracomunicador con los hijos
    MPI_Comm ParesImpares, ParesImpares2, Pares, Impares; // Intercomunicador con los grupos
    time_t t;
    int *pares, *impares;
    int rangos[PROCESOS/2], rangos2[PROCESOS/2];; 
    int maximo, minimo, i; 
    int rank;
    int valor;
    int gr_local, gr_remoto;
	
    MPI_Group grupo_padre, grupo_hijos, grupo_hijos_separados, grupo_hijos_separados2;
	
    pares = (int*) malloc(sizeof(int)*PROCESOS);
    impares = (int*) malloc(sizeof(int)*PROCESOS);
	
    // Inicia MPI
    MPI_Init(&argc, &argv);
    
    // Obtiene su rango y número de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &procesos); 
    MPI_Comm_rank(MPI_COMM_WORLD, &miRango);
    valor = 0;

    // Semilla para generar secuencias aleatorias
    srand((unsigned) time(&t));


    MPI_Comm_get_parent(&intercom); // Para conocer si tiene padre o no
    	
    if ((procesos>1)&&(intercom == MPI_COMM_NULL)) {
	printf(" Hay más de un proceso maestro, por lo que es erróneo. Se cierra el programa. \n");
	MPI_Finalize();
	return -1;
    }
    	
    //printf( " %d  %d \n", sizeof (long int), sizeof (MPI_INT));
    if (intercom == MPI_COMM_NULL) {
	// Se crean los procesos hijos
	MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, PROCESOS, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
		
	// Se crean los números. 
	for (i=0; i<PROCESOS/2; i++) {
	    pares[i] = i*2;
	    printf("Pares %d \n", pares[i]);			
	}
	for (i=0; i<PROCESOS/2; i++) {
	    impares[i] = i*2+1;		
	    printf("Impares %d \n", impares[i]);
	}
		
	// duplicamos el comunicador intercom por claridad a la hora
	// de crear los dos intercomunicodores con los pares e impares
	// es colectiva, por lo que los hijos deben llamarla también
	MPI_Comm_dup(intercom, &intercom2);
   	 	
	//Creamos un grupo local con el padre a partir del intercomunicador
	MPI_Comm_group(intercom, &grupo_padre);
	MPI_Group_size(grupo_padre, &rank);
			
	// Creamos el intercomunicador para los pares, de esta manera 
	// aquí estamos determinando el grupo local del intercomunicador
	// de los procesos pares, los hijos harán el otro grupo.
	MPI_Comm_create(intercom, grupo_padre, &Pares);
	// Liberamos el grupo
	MPI_Group_free(&grupo_padre);
		
	// ahora podemos comprobar el tamaño del grupo local y remoto
	MPI_Comm_size(Pares, &gr_local);
	MPI_Comm_remote_size(Pares, &gr_remoto);
	printf("Soy el padre, en el intercomunicador de Pares somos %d  procesos en el grupo local y hay %d procesos en el remoto \n", gr_local, gr_remoto);
	
	// Creamos el intercomunicador para los impares de la misma manera
	// pero usando el intercomunicador duplicado, no es necesario 
	// y podría hacerse sobre el mismo
		
	// Primero el grupo local del nuevo intercomunicador con el padre
	MPI_Comm_group(intercom2, &grupo_padre);
	// Creamos el intercomunicador en el lado del padre
	MPI_Comm_create(intercom2, grupo_padre, &Impares); 
	// liberamos el grupo padre
	MPI_Group_free(&grupo_padre);
		
	MPI_Comm_size(Impares, &gr_local);
	MPI_Comm_remote_size(Impares, &gr_remoto);
	printf("Soy el padre, en el intercomunicador de Impares somos %d  procesos en el grupo local y hay %d procesos en el remoto \n", gr_local, gr_remoto);
			
	// Se envían los datos a los pares e impares con un scatter
	MPI_Scatter(pares, 1, MPI_INT, &valor, 1, MPI_INT, MPI_ROOT, Pares);
	MPI_Scatter(impares, 1, MPI_INT, &valor, 1, MPI_INT, MPI_ROOT, Impares);
	
	// Ahora con dos Reduce ya sabremos cuáles son los máximo y mínimo
	MPI_Reduce(&valor, &maximo, 1, MPI_INT, MPI_MAX, MPI_ROOT, Pares);
	MPI_Reduce(0, &minimo, 1, MPI_INT, MPI_MIN, MPI_ROOT, Impares);
		
	printf("El valor máximo de los procesos pares es %d.\n",maximo);
	printf("El valor mínimo de los procesos impares es %d.\n", minimo);
	MPI_Comm_free(&Pares);
	MPI_Comm_free(&Impares);	
		
    } else { //no es el proceso padre, es un hijo
	// Duplicamos el intercomunicador para crear los intercomunicadores, 
	// pero es por claridad, no sería necesario
	MPI_Comm_dup(intercom, &intercom2);
		
	// Creamos los grupos indicando los rangos de los procesos que 
	// pertenecerán a cada uno
	for (i=0; i<PROCESOS/2; i++) {
	    rangos[i] = i*2; // rangos pares
	    rangos2[i] = i*2+1; // rangos impares
	}
	// Ahora creamos los grupos separados dependiendo de si el proceso es par o impar
		
	// Creamos el grupo de hijos completo
	MPI_Comm_group(intercom,&grupo_hijos);
	// Creamos el grupo de los pares
	MPI_Group_incl(grupo_hijos, PROCESOS/2, rangos, &grupo_hijos_separados);
	MPI_Group_free(&grupo_hijos);
	// Creamos el intercomunicador de los pares, se ha cambiado la variable
	// para remarcar que no tiene que ser la misma que en el padre
	// y que el creado en el padre aún no existe en los hijos hasta crearlo
	// en este lado del comunicador
	MPI_Comm_create(intercom, grupo_hijos_separados, &ParesImpares);
	MPI_Group_free(&grupo_hijos_separados);
		
	// Repetimos para los impares con el intercomunicador duplicado
		
	// Creamos el grupo de hijos completo
	MPI_Comm_group(intercom2,&grupo_hijos);
	// Creamos el grupo de impares
	MPI_Group_incl(grupo_hijos, PROCESOS/2, rangos2, &grupo_hijos_separados2);
	MPI_Group_free(&grupo_hijos);		
	// Creamos el intercomunicador de los impares
	MPI_Comm_create(intercom2, grupo_hijos_separados2, &ParesImpares2);	
	MPI_Group_free(&grupo_hijos_separados2);
		
	// Recibimos los valores del scatter correspondiente
	// Si hubieramos usado la misma variable para los comunicadores 
	// de pares e impares, el siguiente if sería innecesario ya que
	// aunque tuvieran la misma variable, serían diferentes y cada uno
	// llamaría al suyo, pero se han puesto diferentes y se 
	// ha duplicado el código para clarificarlo. Podéis probar a simplificarlo
	// usando los mismos y ver que se obtiene lo mismo
	if (miRango%2==0) { //par
	    MPI_Comm_rank(ParesImpares, &rank);
	    printf("Soy el hijo %d y en el nuevo comunicador soy %d \n", miRango, rank);
	    // Se recibe el valor con Scatter
	    MPI_Scatter(0, 0, MPI_INT, &valor, 1, MPI_INT, 0, ParesImpares);
	    printf("Soy el hijo %d y he recibido %d \n", miRango, valor);
	    valor = valor*(rand()%100);
	    // Se envía al maestro con Reduce
	    MPI_Reduce(&valor, &maximo, 1, MPI_INT, MPI_MAX, 0, ParesImpares);
	    MPI_Comm_free(&ParesImpares);
	}	else {
	    MPI_Comm_rank(ParesImpares2, &rank);
	    printf("Soy el hijo %d y en el nuevo comunicador soy %d \n", miRango, rank);
	    // Se recibe el valor con Scatter
	    MPI_Scatter(0, 0, MPI_INT, &valor, 1, MPI_INT, 0, ParesImpares2);
	    printf("Soy el hijo %d y he recibido %d \n", miRango, valor);
	    valor = valor*(rand()%100);
	    // Se envía al maestro con Reduce
	    MPI_Reduce(&valor, 0, 1, MPI_INT, MPI_MAX, 0, ParesImpares2);
	    MPI_Comm_free(&ParesImpares2);
	}
			
    } //fin del if para distinguir el proceso original
    MPI_Comm_free(&intercom);
    MPI_Comm_free(&intercom2);
    MPI_Finalize();
    return 0;

} //Cerrar main
