/******************************************************************************
 * Fichero: Ejemplo2.c
 * Ejemplo de funcionamiento de rutinas colectivas con Gather y Scatter
 * El proceso 0 manda un valor aleatorio al resto de procesos
 * estos lo multiplican por -1 y lo devuelven al maestro.
 *
 * Compilar con:    mpicc Ejemplo2.c -o Ejemplo2
 * Ejecutar con:    mpirun -np X Ejemplo2
 * X puede ser cualquier número entero positivo mayor que 1
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
    //Declaración de variables a usar en el programa
    int myrank, nprocs, i;
    int *valores;
    int recibido;
    int *valoresfinal;
	
    //Inicialización de MPI, cada proceso obtiene en nº de procesos y su rango
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
    // Semilla para generar secuencias aleatorias
    time_t t;
    srand((unsigned) time(&t));

    valoresfinal = (int *)malloc(nprocs*sizeof(int));
    
    //Si solo existe un proceso, el programa finaliza
    if (nprocs==1) {
	printf(" Solo existe un proceso por lo que finaliza el ejemplo. \n");
	MPI_Finalize();
	return 0;
    }
	
    //Reservamos memoria y generamos el vector de valores a repartir
    valores = (int *) malloc(nprocs*sizeof(int));
    if (myrank == 0){
	for (i=0; i<nprocs; i++) {
	    valores[i] = 1+rand()%100;		
	}
    }
	
    //Mandamos un valor de los generados a cada proceso
    MPI_Scatter(valores, 1, MPI_INT, &recibido, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Hola desde el proceso de rango %d de un total de %d\n", myrank, nprocs);
    printf("Soy el proceso %d y he recibido %d del maestro \n", myrank, recibido);
    recibido = recibido*(-1);
    //Se reciben los valores modificados por parte del proceso 0, que es el root
    MPI_Gather(&recibido, 1, MPI_INT, valoresfinal, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (myrank == 0){
	for (i=1; i<nprocs; i++) {
	    printf("Hola, soy %d y he recibido del proceso %d el valor %d \n", myrank, i, valoresfinal[i]);		
	}
    }
	
    MPI_Finalize();
    return 0;
}
