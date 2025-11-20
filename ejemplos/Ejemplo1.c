/***************************************************************************
 * Fichero: Ejemplo1.c
 * Ejemplo de funcionamiento de primitivas de MPI y universos de comunicación de los comunicadores
 * Compilar con:    mpicc Ejemplo1.c –o Ejemplo1
 * Ejecutar con:    mpirun -np X Ejemplo1
Donde X es un número entero mayor que 0.
***************************************************************************/

#include <stdio.h>
#include <mpi.h>
int main(int argc, char** argv)
{
    int myrank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
    printf("Hola soy el proceso %d de un total de %d\n", myrank, nprocs);
    MPI_Finalize();
    return 0;
}
