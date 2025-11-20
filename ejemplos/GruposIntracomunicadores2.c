/******************************************************************************
 * Fichero: GruposIntracomunicadores2.c
 *
 * Compilar con:    mpicc GruposIntracomunicadores2.c -o intra2
 * Ejecutar con:    mpirun -np X intra2
 * siendo X un número entero mayor que 1. Preferiblemente impar para ver
 * la diferencia en el número de procesos por intracomunicador

 
 * Creamos de forma estática un conjunto de procesos, se crearán dos grupos
 * de procesos, uno para los de rango par y otro para los de impar. Una vez
 * creados los grupos, se creará un intracomunicador para cada uno. En este caso
 * la creación de los comunicadores sí es colectiva, por lo que todos los 
 * procesos del comunicador original deben llamar a la función de creación
 * de los nuevos, vayan a pertencer o no a ellos.  Sin embargo una vez creados,
 * sólo los procesos que pertenecen a los comunicadores pueden hacer llamadas a ellos
 * 
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
    MPI_Comm comPar, comImpar;
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
	
    // Creamos los grupos par e impar
    MPI_Group_incl(grupo_completo, n_par, rangos_par, &grupo_par);
    MPI_Group_incl(grupo_completo, n_impar, rangos_impar, &grupo_impar);
    // Creamos los nuevos intracomunicadores de procesos pares e impares
    // a partir del MPI_COMM_WORLD, por lo que todos los miembros de 
    // MPI_COMM_WORLD deben llamar a las funciones.
    MPI_Comm_create(MPI_COMM_WORLD, grupo_par, &comPar);
    MPI_Comm_create(MPI_COMM_WORLD, grupo_impar, &comImpar);

    if (myrank%2==0){ // proceso spares
	// Comprobamos tamaño y rango del grupo par.
	MPI_Group_size(grupo_par, &n_par);
	MPI_Group_rank(grupo_par, &rankpar);
	printf(" Soy %d. El grupo par %d elementos y tengo rango %d \n", myrank, n_par, rankpar);
	// Comprobamos tamaño y rango del intracomunicador par. Estas funciones
	// solo pueden llamarlas los procesos que pertencen a dichos comunicadores
	MPI_Comm_size(comPar, &n_par);
	MPI_Comm_rank(comPar, &rankpar);
	printf(" Soy %d. El comunicador par %d elementos y tengo rango %d \n", myrank, n_par, rankpar);
    } else {
	// Comprobamos tamaño y rango del grupo par.
	MPI_Group_size(grupo_impar, &n_impar);
	MPI_Group_rank(grupo_impar, &rankimpar);
	printf(" Soy %d. El grupo impar %d elementos y tengo rango %d \n", myrank, n_impar, rankimpar);
	// Comprobamos tamaño y rango del intracomunicador par. Estas funciones
	// solo pueden llamarlas los procesos que pertencen a dichos comunicadores
	MPI_Comm_size(comImpar, &n_impar);
	MPI_Comm_rank(comImpar, &rankimpar);
	printf(" Soy %d. El comunicador impar %d elementos y tengo rango %d \n", myrank, n_impar, rankimpar);
    }

    MPI_Finalize();
    return 0;
}
