/******************************************************************************
 * Fichero: GruposIntracomunicadores.c
 *
 * Compilar con:    mpicc GruposIntracomunicadores.c -o intra1
 * Ejecutar con:    mpirun -np X intra1
 * siend X un número entero mayor que 1

 
 * Creamos de forma estática un conjunto de procesos, se crearán dos grupos
 * de procesos, uno para los de rango par y otro para los de impar. La creación
 * de grupos no es colectiva, por lo que se muestran dos alternativas para
 * crear los grupos, una comentada y otra sin comentar. 
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int myrank, nprocs;
    int gproc, n_par, n_impar;
    int i, *rangos_par, *rangos_impar;
    int rankpar, rankimpar;
    MPI_Group grupo_completo, grupo_par, grupo_impar;
	
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
    rangos_impar = (int *) malloc(nprocs*sizeof(int));
    n_par = 0;
    n_impar = 0;
    for (i=0; i<nprocs; i++) {
	if (i%2==0){
	    rangos_par[n_par] = i;
	    n_par = n_par+1;
	} else {
	    rangos_impar[n_impar] = i;
	    n_impar = n_impar+1;
	}
    }
	
    // Creamos los grupos llamando todos los procesos a todas las funciones
    // COMENTADO
	
    /*	MPI_Group_incl(grupo_completo, n_par, rangos_par, &grupo_par);
	MPI_Group_incl(grupo_completo, n_impar, rangos_impar, &grupo_impar);
	
	MPI_Group_size(grupo_par, &n_par);
	MPI_Group_rank(grupo_par, &rankpar);
	MPI_Group_size(grupo_impar, &n_impar);
	MPI_Group_rank(grupo_impar, &rankimpar);
	
	printf(" Soy %d. El grupo par %d elementos y tengo rango %d \n", myrank, n_par, rankpar);
	printf(" Soy %d. El grupo impar %d elementos y tengo rango %d \n", myrank, n_impar, rankimpar);
    */
	
    // Creamos los grupos llamando sólo cada proceso a las funciones para
    // crear su grupo
    if (myrank%2==0){
	MPI_Group_incl(grupo_completo, n_par, rangos_par, &grupo_par);
	MPI_Group_size(grupo_par, &n_par);
	MPI_Group_rank(grupo_par, &rankpar);
	printf(" Soy %d. El grupo par %d elementos y tengo rango %d \n", myrank, n_par, rankpar);
	
    } else {
	MPI_Group_incl(grupo_completo, n_impar, rangos_impar, &grupo_impar);
	MPI_Group_size(grupo_impar, &n_impar);
	MPI_Group_rank(grupo_impar, &rankimpar);
	printf(" Soy %d. El grupo impar %d elementos y tengo rango %d \n", myrank, n_impar, rankimpar);
    }
	
    MPI_Finalize();
    return 0;
}
