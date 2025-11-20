/******************************************************************************
 * Fichero: Intercomunicadores1.c
 *
 * Compilar con:    mpicc Intercomunicadores1.c -o inter1
 * Ejecutar con:    mpirun -np 1 inter1

 * Enunciado: 
 
 * Un proceso maestro genera de manera dinámica un número de procesos. 
 * A su vez, este proceso maestro generará números aleatorios y se los enviará a los procesos con rango par, 
 * que lo multiplicarán por otro número aleatorio. Luego el maestro creará otros números aleatorios y 
 * se los enviará a los procesos de rango impar, que también lo multiplicarán por un número aleatorio. 
 * Finalmente el proceso maestro debera conocer cuál es el mayor valor resultante de las multiplicaciones en 
 * cada uno de los procesos de rango par, y el menor valor de entre los procesos de rango impar.
 * 
 * NOTA: EL número de procesos hijos se considera par por simplicidad
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define PROCESOS 10

/******************************************************************************
 * Programa principal: Alternativa 1
 *****************************************************************************/
int main(int argc, char *argv[]) {
    // Declaración de variables
    int procesos;			
    int miRango;                 
    MPI_Comm intercom;          
    time_t t;
    float numeros[PROCESOS];
    int i, maximo, minimo;
    float Valor;
    
    // Inicia MPI
    MPI_Init(&argc, &argv);
    
    // Obtiene su rango y número de procesos, y verifica que son los adecuados
    MPI_Comm_size(MPI_COMM_WORLD, &procesos); 
    MPI_Comm_rank(MPI_COMM_WORLD, &miRango);
    
    
    // Semilla para generar secuencias aleatorias
    srand((unsigned) time(&t));
    
    
    MPI_Comm_get_parent(&intercom); // Para conocer si tiene padre o no
    
    if (intercom == MPI_COMM_NULL) {
	// Se crean los procesos hijos
	if (procesos>1) {
	    printf(" Hay más de un proceso maestro, por lo que es erróneo. Se cierra el programa. \n");
	    MPI_Finalize();
	    return -1;
	}
	MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, PROCESOS, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
	
	// Se envían los datos, primero a los pares, luego a los impares. Comunicación punto a punto. 
	for (i=0; i<PROCESOS/2; i++) {
	    Valor = 1+rand()%100;			
	    MPI_Send(&Valor, 1, MPI_FLOAT, i*2, 2, intercom);
	}
	for (i=0; i<PROCESOS/2; i++) {
	    Valor = 1+rand()%100;			
	    MPI_Send(&Valor, 1, MPI_FLOAT, i*2+1, 1, intercom);
	}
	// Hacemos un gather para recoger de todos los valores y luego buscamos entre pares e impares
	MPI_Gather(&Valor, 1, MPI_FLOAT, numeros, 1, MPI_FLOAT, MPI_ROOT, intercom);  
	
	// Ahora recorremos el vector de números y buscamos los valores.
	maximo = 0;
	minimo = 10000;
	for (i=0; i<PROCESOS/2; i++) {
	    if (numeros[2*i]>maximo) maximo = numeros[2*i];
	}
	for (i=0; i<PROCESOS/2; i++) {
	    if (numeros[2*i+1]<minimo) minimo = numeros[2*i+1];
	}	
	printf("El valor máximo de los procesos pares es %d.\n", maximo);
	printf("El valor mínimo de los procesos impares es %d.\n", minimo);
	
    } else { //no es el proceso padre, es un hijo
	// Se reciben los valores y se multiplica por un valor aleatorio
	MPI_Recv(&Valor, 1, MPI_FLOAT, 0, MPI_ANY_TAG, intercom, MPI_STATUS_IGNORE);
	Valor = Valor*(rand()%100);
	printf("Valor = %f del proceso %d \n",Valor, miRango);
	// Hacemos un gather para enviar los valores al padre a través del intercomunicador
	MPI_Gather(&Valor, 1, MPI_FLOAT, numeros, 1, MPI_FLOAT, 0, intercom);
	
    } //fin del if para distinguir el proceso original
    
    MPI_Finalize();
    return 0;
    
} //Cerrar main



