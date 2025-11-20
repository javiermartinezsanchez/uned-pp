/******************************************************************************
 * Fichero: HolaMundo.c
 *
 * Compilar con:    mpicc HolaMundo.c -o hola
 * Ejecutar con:    mpirun -np 2 hola

 
* Dos procesos se arrancan de forma estática, cada uno de ellos pedirá
* un dato por entrada estándar y se sincronizarán con una barrera de manera
* que el proceso 0 pida el dato antes que el 1. 

 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int myrank, nprocs, prueba;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
    printf("Hola desde %d de %d\n", myrank, nprocs);
	if (myrank == 0){
	printf("\n Introduce dato para proceso %d: \n", myrank);
	scanf("%d",&prueba);
	printf("\n dato 0 %d \n", prueba);
	MPI_Barrier(MPI_COMM_WORLD);
	}
	if (myrank == 1){
	MPI_Barrier(MPI_COMM_WORLD);
	printf("\n Introduce dato para proceso %d: \n", myrank);
	scanf("%d",&prueba);
	printf("\n dato 1 %d \n", prueba);
	printf("ACLARACIÓN: stdin está redirigido únicamente al proceeso de rango 0, por lo que el dato para el proceso 1 no se lee correctamente\n");
	}

MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
