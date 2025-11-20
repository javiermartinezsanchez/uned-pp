/******************************************************************************
 * Fichero: GruposIntracomunicadores3.c
 *
 * Compilar con:    mpicc GruposIntracomunicadores3.c -o intra3
 * Ejecutar con:    mpirun -np X intra3
 * siendo X un número entero mayor que 1. Preferiblemente impar para ver
 * la diferencia en el número de procesos por intracomunicador

 * Creamos de forma estática un conjunto de procesos, se crearán dos grupos
 * de procesos, uno para los de rango par y otro para los múltiplos de tres
 * para ver como crear comunicadores que se solapan en los procesos. Una vez
 * creados los grupos, se creará un intracomunicador para cada uno. En este caso
 * la creación de los comunicadores sí es colectiva, por lo que todos los 
 * procesos del comunicador original deben llamar a la función de creación
 * de los nuevos, vayan a pertenecer o no a ellos.
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int myrank, nprocs;
    int gproc, n_par, n_tres;
    int i, *rangos_par, *rangos_tres;
    int rankpar, ranktres;
    MPI_Comm comPar, comTres;
    MPI_Group grupo_completo, grupo_par, grupo_tres;
	
    // Inicializamos MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
    // Creamos un grupo con todos los procesos para luego dividirlo
    MPI_Comm_group(MPI_COMM_WORLD, &grupo_completo);
    MPI_Group_size(grupo_completo, &gproc);
    printf(" El grupo completo %d elementos \n", gproc);
	
    //Determinamos nº elementos y rangos de cada grupo
    rangos_par = (int *) malloc(nprocs*sizeof(int));
    rangos_tres = (int *) malloc(nprocs*sizeof(int));
    n_par = 0;
    n_tres = 0;
    for (i=0; i<nprocs; i++) {
	if (i%2==0){
	    rangos_par[n_par] = i;
	    n_par = n_par+1;
	}
	if (i%3==0) {
	    rangos_tres[n_tres] = i;
	    n_tres = n_tres+1;
	}
    }
	
    // Creamos los grupos e intracomunicadores
    MPI_Group_incl(grupo_completo, n_par, rangos_par, &grupo_par);
    MPI_Comm_create(MPI_COMM_WORLD, grupo_par, &comPar);
    MPI_Group_incl(grupo_completo, n_tres, rangos_tres, &grupo_tres);
    MPI_Comm_create(MPI_COMM_WORLD, grupo_tres, &comTres);
	
    if (myrank%2==0){
	MPI_Group_size(grupo_par, &n_par);
	MPI_Group_rank(grupo_par, &rankpar);
	printf(" Soy %d. El grupo par %d elementos y tengo rango %d \n", myrank, n_par, rankpar);
	MPI_Comm_size(comPar, &n_par);
	MPI_Comm_rank(comPar, &rankpar);
	printf(" Soy %d. El comunicador par %d elementos y tengo rango %d \n", myrank, n_par, rankpar);
    }
	
    // En este caso debe haber dos if ya que hay procesos que pueden estar
    // en ambos comunicadores
    if (myrank%3==0){
	MPI_Group_size(grupo_tres, &n_tres);
	MPI_Group_rank(grupo_tres, &ranktres);
	printf(" Soy %d. El grupo TRES %d elementos y tengo rango %d \n", myrank, n_tres, ranktres);
	MPI_Comm_size(comTres, &n_tres);
	MPI_Comm_rank(comTres, &ranktres);
	printf(" Soy %d. El comunicador TRES %d elementos y tengo rango %d \n", myrank, n_tres, ranktres);
    }

    MPI_Finalize();
    return 0;
}
