/******************************************************************************
 * Fichero: Intercomunicadores5.c
 *
 * Compilar con:    mpicc Intercomunicadores5.c -o inter5
 * Ejecutar con:    mpirun -np 1 inter5

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
 
#define PROCESOS 10
/******************************************************************************
 * Programa principal: Creación de intercomunicadores con Split
 *****************************************************************************/
int main(int argc, char *argv[]) {
    // Declaración de variables
    int procesos;		
    int miRango;                        
    MPI_Comm intercom, intercom2;          
    MPI_Comm Pares, Impares;            
    time_t t;
    int pares[PROCESOS/2], impares[PROCESOS/2]; 
    int maximo, minimo, i, valor, color;

    // Inicialización de MPI
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
	// Se crean los procesos hijos
	MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, PROCESOS, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
		
	// Se crean los números. 
	for (i=0; i<PROCESOS/2; i++) {
	    pares[i] = 1+rand()%100;		
	}
	for (i=0; i<PROCESOS/2; i++) {
	    impares[i] = 1+rand()%100;			
	}
	// Duplicamos el intercomunicador por claridad en la creación
	// de los nuevos intercomunicadores
	MPI_Comm_dup(intercom, &intercom2);
       
	// Creamos comunicadores, el primero para los pares y el segundo
	// para los impares. Son dos llamadas a split por lo que los hijos
	// deben hacer también dos llamadas a split, uno con color en su comunicador
	// deseado de salida y la otra con MPI_UNDEFINED. Estas dos llamadas
	// son necesarias ya que para crear el intercomunicador el maestro
	// debe tener el mismo color que los hijos, por lo que para crear
	//  los dos debe llamar a split con ambos colores. 
	MPI_Comm_split(intercom, 0, miRango, &Pares);
	MPI_Comm_split(intercom2, 1, miRango, &Impares);
	
	// Se envían los datos a los pares e impares con un scatter en 
	// sus correspondientes intercomunicadores
	MPI_Scatter(pares, 1, MPI_INT, &valor, 1, MPI_INT, MPI_ROOT, Pares);
	MPI_Scatter(impares, 1, MPI_INT, &valor, 1, MPI_INT, MPI_ROOT, Impares);
		
	// Ahora con dos Reduce ya sabremos cuáles son los máximo y mínimo
	MPI_Reduce(0, &maximo, 1, MPI_INT, MPI_MAX, MPI_ROOT, Pares);
	MPI_Reduce(0, &minimo, 1, MPI_INT, MPI_MIN, MPI_ROOT, Impares);

	printf("El valor máximo de los procesos pares es %d.\n", maximo);
	printf("El valor mínimo de los procesos impares es %d.\n", minimo);
		
    } else { //no es el proceso original, es un hijo
	// Con color distinguimos entre pares e impares
	color = miRango%2;
	// Al igual que en el maestro duplicamos el intercomunicador 
	// para que todos los procesos involucrados lo tengan
	MPI_Comm_dup(intercom, &intercom2);
	// Creamos los intercomunicadores
	if (color == 0) { //pares
	    // Los pares crean su comunicador de pares
	    MPI_Comm_split(intercom, color, miRango, &Pares);
	    // y llaman al split para crear el comunicador de impares
	    // con color = MPI_UNDEFINED ya que para esta llamada no 
	    // deseamos que los pares creen comunicador alguno.
	    MPI_Comm_split(intercom2, MPI_UNDEFINED, miRango, &Impares);
	    // Recibe los valores del padre
	    MPI_Scatter(pares, 1, MPI_INT, &valor, 1, MPI_INT, 0, Pares);			
	    valor = valor*(rand()%100);
	    // Se devuelve para calcular el máximo
	    MPI_Reduce(&valor, 0, 1, MPI_INT, MPI_MAX, 0, Pares);
	} else {
	    // Los impares llaman a split con MPI_UNDEFINED en color
	    // para el comunicador de los pares
	    MPI_Comm_split(intercom, MPI_UNDEFINED, miRango, &Pares);
	    // Y después crean su intercomunicador de impares
	    MPI_Comm_split(intercom2, color, miRango, &Impares);
	    // Se reciben los datos
	    MPI_Scatter(impares, 1, MPI_INT, &valor, 1, MPI_INT, 0, Impares);
	    valor = valor*(rand()%100);
	    // Se devuelven al padre para calcular el mínimo
	    MPI_Reduce(&valor, 0, 1, MPI_INT, MPI_MIN, 0, Impares);
	}
    } //fin del if para distinguir el proceso original
		
    MPI_Finalize();
    return 0;

} //Cerrar main
