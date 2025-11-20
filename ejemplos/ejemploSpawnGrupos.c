/******************************************************************************
 * Fichero: ejemploSpawnGrupos.c
 *
 * Compilar con:    mpicc HejemploSpawnGrupos.c -o spawn
 * Ejecutar con:    mpirun -np 1 spawn

 
 * Un proceso padre creará un número determinado de hijos y estos, de 
 * forma local, crearán un grupo entre ellos

 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>

#define PROCESOS 3

int main(int argc, char** argv)
{
    int myrank, nprocs;
    int procesos;
    MPI_Comm intercom;
    MPI_Group grupo_hijos;
	
    // Inicializamos MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
    MPI_Comm_get_parent(&intercom); // Para conocer si tiene padre o no
    if (intercom == MPI_COMM_NULL) {
	// Se crean los procesos hijos
	if (nprocs>1) {
	    printf(" Solo puede existir un proceso por lo que finaliza el ejemplo. \n");
	    MPI_Finalize();
	    return 0;
	}
	MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, PROCESOS, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
	
    } else {
	// Se crea un grupo con los procesos del grupo local del intercomunicador
	MPI_Comm_group(intercom, &grupo_hijos);
	MPI_Group_size(grupo_hijos, &procesos);
	printf(" El grupo de hijos tiene %d elementos \n", procesos);
	
    }
    MPI_Finalize();
    return 0;
}
