/* Asignatura: Procesamiento Paralelo
 * Grado: Tecnologías de la Información
 * Departamento: Informática y Automática, ETSI Informática, UNED
 * Autores: David Moreno Salinas, Victorino Sanz Prat
 * Nombre: Ejercicio1.c
 * Explicación:
 * Creación de N procesos de forma estática. Muestra por salida estándar
 * el rango de cada uno de los procesos en el MPI_COMM_WORLD y en el 
 * MPI_COMM_SELF. Se realiza una comunicación punto a punto entre el 
 * proceso de rango 0 y el de rango N-1. Pretende mostrar como los 
 * procesos estáticos pertenecen todos al MPI_COMM_WORLD y cada uno tiene
 * un comunicador MPI_COMM_SELF propio.
 * Notas: reproduzca el código y modifíquelo para probar su funcionamiento
 * e introducir la variantes que considere oportunas. 
 * */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int my_world_rank, my_self_rank;
    int nprocs, selfprocs;
    int token;
	// Inicialización de MPI
    MPI_Init(&argc, &argv);
    // Rango y tamaño del MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_world_rank);
    printf("Soy el proceso de rango %d en el MPI_COMM_WORLD, de un total de %d procesos \n",my_world_rank,nprocs);
    // Rango y tamaño del MPI_COMM_SELF
    MPI_Comm_size(MPI_COMM_SELF, &selfprocs);
    MPI_Comm_rank(MPI_COMM_SELF, &my_self_rank);
    printf("Soy el proceso %d con rango %d en mi comunicador MPI_COMM_SELF, de un total de %d procesos \n",my_world_rank, my_self_rank,selfprocs);
    token = 0; 
    // El proceso 0 envía un token al proceso N-1
    if (my_world_rank == 0) {
		token = 50; 
		MPI_Send(&token, 1, MPI_INT, nprocs-1, 99, MPI_COMM_WORLD);
	} else if ( my_world_rank == nprocs-1) {
		MPI_Recv(&token, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	printf("Soy el proceso %d, y el valor del token para mí es %d \n", my_world_rank, token);
    MPI_Finalize();
    return 0;
}
