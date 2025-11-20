/******************************************************************************
 * Fichero: Intercomunicadores3.c
 *
 * Compilar con:    mpicc Intercomunicadores3.c -o inter3
 * Ejecutar con:    mpirun -np 1 Inter3

 * Enunciado: 

 * Un proceso maestro genera de manera dinámica un número de procesos. 
 * A su vez, este proceso maestro generará números aleatorios y se los 
 * enviará a los procesos con rango par, que lo multiplicarán por otro 
 * número aleatorio. Luego el maestro creará otros números aleatorios y 
 * se los enviará a los procesos de rango impar, que también lo 
 * multiplicarán por un número aleatorio. Finalmente el proceso maestro 
 * deberá conocer cuál es el mayor valor resultante de las multiplicaciones 
 * en cada uno de los procesos de rango par, y el menor valor de entre 
 * los procesos de rango impar.
 * 
 * NOTA: EL número de procesos hijos se considera par por simplicidad
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

 #define PROCESOS 10
/******************************************************************************
 * Programa principal
 *****************************************************************************/
int main(int argc, char *argv[]) {
    // Declaración de variables
    int procesos;			// Número de procesos creados de forma estática, que será 1
    int miRango, myrank;                        
    MPI_Comm intercom, iguales;          // Intercomunicador e intracomunicador con los hijos
    MPI_Comm Pares, Impares, paresImpares;             // Intercomunicador con los grupos
    time_t t;
    int pares[PROCESOS/2], impares[PROCESOS/2]; 
    int maximo, minimo, i, valor, color;
    
    // Inicia MPI
    MPI_Init(&argc, &argv);
    
    // Obtiene su rango y número de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &procesos); 
    MPI_Comm_rank(MPI_COMM_WORLD, &miRango);
    
    // Semilla para generar secuencias aleatorias
    srand((unsigned) time(&t));
    
    
    MPI_Comm_get_parent(&intercom); // Para conocer si tiene padre o no
    
    if (intercom == MPI_COMM_NULL) {
	
	if (procesos>1) {
	    printf(" Hay más de un proceso maestro, por lo que es erróneo. Se cierra el programa. \n");
	    MPI_Finalize();
	    return -1;
	}
	// Se crean los procesos Padres
	MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, PROCESOS, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
	
	// Se crean los números. 
	for (i=0; i<PROCESOS/2; i++) {
	    pares[i] = 1+rand()%100;		
	}
	for (i=0; i<PROCESOS/2; i++) {
	    impares[i] = 1+rand()%100;			
	}
	
	//Creamos comunicador de iguales para crear luego los intercomunicadores con los grupos
	MPI_Intercomm_merge(intercom, 0, &iguales);
	
	// Creamos el intercomunicador para los pares en el lado del padre
	MPI_Intercomm_create(MPI_COMM_SELF, 0, iguales, 1, 2, &Pares); 
	
	// Creamos el intercomunicador para los impares en el lado del padre
	MPI_Intercomm_create(MPI_COMM_SELF, 0, iguales, 2, 1, &Impares);
	
	// Se envían los datos a los pares e impares con un scatter 
	// usando el intercomunicador dedicado a cada grupo
	MPI_Scatter(pares, 1, MPI_INT, &valor, 1, MPI_INT, MPI_ROOT, Pares);
	MPI_Scatter(impares, 1, MPI_INT, &valor, 1, MPI_INT, MPI_ROOT, Impares);
	
	// Ahora con dos Reduce ya sabremos cuáles son los máximo y mínimo
	MPI_Reduce(0, &maximo, 1, MPI_INT, MPI_MAX, MPI_ROOT, Pares);
	MPI_Reduce(0, &minimo, 1, MPI_INT, MPI_MIN, MPI_ROOT, Impares);
	
	printf("El valor máximo de los procesos pares es %d.\n", maximo);
	printf("El valor mínimo de los procesos impares es %d.\n", minimo);
	
    } else { //no es el proceso padre, es un hijo
	// Comunicador de iguales para crear el intercomunicador
	MPI_Intercomm_merge(intercom, 1, &iguales);
	// Dividimos el grupo local en dos intracomunicadores diferentes
	// aunque con la misma variable, para crear los intercomunicadores
	// con el proceso padre
	color = miRango%2;
	MPI_Comm_split(MPI_COMM_WORLD, color, miRango, &paresImpares);
	// Creamos los intercomunicadores para cada grupo de procesos
	// Podría haberse usado la misma variable para ambos intercomunicadores
	// en los hijos y se hubieran ahorrado líneas de código, pero se ha
	// separado para mostrar claramente como hay dos intercomunicadores
	if (color == 0) { //pares
	    MPI_Intercomm_create(paresImpares, 0, iguales, 0, 2, &Pares);
	    MPI_Scatter(pares, 1, MPI_INT, &valor, 1, MPI_INT, 0, Pares);
	    // Obtengo el rango local en el nuevo intercomunicador
	    MPI_Comm_rank(Pares, &myrank);
	    printf("Valor = %d del proceso con rango %d en MPI_COMM_WORLD y rango %d en Pares \n",valor, miRango, myrank);			
	    valor = valor*(rand()%100);
	    MPI_Reduce(&valor, 0, 1, MPI_INT, MPI_MAX, 0, Pares);
	} else {
	    MPI_Intercomm_create(paresImpares, 0, iguales, 0, 1, &Impares);
	    MPI_Scatter(impares, 1, MPI_INT, &valor, 1, MPI_INT, 0, Impares);
	    // Obtengo el rango local en el nuevo intercomunicador
	    MPI_Comm_rank(Impares, &myrank);
	    printf("Valor = %d del proceso con rango %d en MPI_COMM_WORLD y rango %d en Impares \n",valor, miRango, myrank);		
	    valor = valor*(rand()%100);
	    MPI_Reduce(&valor, 0, 1, MPI_INT, MPI_MIN, 0, Impares);
	}
	
    } //fin del if para distinguir el proceso original
    
    MPI_Finalize();
    return 0;
    
} //Cerrar  main



