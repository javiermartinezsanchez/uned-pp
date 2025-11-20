/******************************************************************************
 * Fichero: Intercomunicadores2.c
 *
 * Compilar con:    mpicc Intercomunicadores2.c -o inter2
 * Ejecutar con:    mpirun -np 1 inter2

 * Enunciado: 

 * Un proceso maestro genera de manera dinámica un número de procesos. 
 * A su vez, este proceso maestro generará números aleatorios y se los enviará a los procesos con rango par, 
 * que lo multiplicarán por otro número aleatorio. Luego el maestro creará otros números aleatorios y 
 * se los enviará a los procesos de rango impar, que también lo multiplicarán por un número aleatorio. 
 * Cada proceso recogerá los valores de su grupo (par o impar), los sumará y enviará al padre. El 
 * padre sumará los valores enviados por los pares y restará los enviados por los impares. 

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
    int procesos;			
    int miRango;                        
    MPI_Comm intercom, ParesImpares;          
    time_t t;
    int numeros[PROCESOS];
    int i, Suma, Resta;
    int Valor, *valores;
    int color;
    int procesosdivididos;
    
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
	    MPI_Send(&Valor, 1, MPI_INT, i*2, 2, intercom);
	}
	for (i=0; i<PROCESOS/2; i++) {
	    Valor = 1+rand()%100;			
	    MPI_Send(&Valor, 1, MPI_INT, i*2+1, 1, intercom);
	}
	// Hacemos un gather para recoger todos los valores
	// numeros = (int *) malloc(PROCESOS*sizeof(int));
	MPI_Gather(&Valor, 1, MPI_INT, numeros, 1, MPI_INT, MPI_ROOT, intercom);  
	
	// Ahora recorremos el vector de números y sumamos y restamos para pares e impares
	Suma = 0;
	Resta = 0;
	for (i=0; i<PROCESOS/2; i++) {
	    Suma  = Suma + numeros[2*i];
	}
	for (i=0; i<PROCESOS/2; i++) {
	    Resta  = Resta - numeros[2*i+1];
	}	
	printf("El valor suma de los procesos pares es %d.\n", Suma);
	printf("El valor resta de los procesos impares es %d.\n", Resta);
	
    } else { //no es el proceso original, es un hijo
	// Se reciben los valores y se hacen dos intracomunicadores separados
	// para comunicar pares e impares de manera independiente
	MPI_Recv(&Valor, 1, MPI_INT, 0, MPI_ANY_TAG, intercom, MPI_STATUS_IGNORE);
	Valor = Valor*(rand()%100);
	printf("Valor = %d del proceso %d \n",Valor, miRango);
	if (miRango%2 == 0) color = 0;
	else color=1;
	// dividimos el comunicador de los hijos en dos, le podemos poner el mismo nombre
	// pero teniendo en cuenta que los pares tienen uno
	// y los impares otro, y no se pueden comunicar entre ellos con ese comunicador
	MPI_Comm_split(MPI_COMM_WORLD, color, miRango, &ParesImpares);
	MPI_Comm_size(ParesImpares, &procesosdivididos);
	printf("Soy el hijo %d y en mi grupo hay %d procesos.\n", miRango,procesosdivididos);
	valores = (int *) malloc(procesosdivididos*sizeof(int));
	//Todos los procesos guardan los valores de su grupo
	MPI_Allgather(&Valor, 1, MPI_INT, valores, 1, MPI_INT, ParesImpares);
	Valor = 0;
	//sumamos los valores que ha recogido cada proceso
	for (i=0; i<procesosdivididos; i++) {
	    Valor = Valor+valores[i];
	}
	printf("Valor final = %d, proceso %d \n",Valor, miRango);
	// Hacemos un gather para recoger todos los valores
	MPI_Gather(&Valor, 1, MPI_INT, numeros, 1, MPI_INT, 0, intercom);
	
    } //fin del if para distinguir el proceso original
    
    MPI_Finalize();
    return 0;
    
} //Cerrar corchete del main


