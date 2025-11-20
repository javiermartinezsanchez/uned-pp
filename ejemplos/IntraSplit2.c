/******************************************************************************
 * Fichero: IntraSplit2.c
 *
 * Compilar con:    mpicc IntraSplit2.c -o split2
 * Ejecutar con:    mpirun -np X split2
 * siendo X un número entero mayor que 1. Preferiblemente impar para ver
 * la diferencia en el número de procesos por intracomunicador
 
 * Creamos de forma estática un conjunto de procesos, y se crearán
 * dos intracomunicadores directamente con MPI_Comm_split. Todos los 
 * procesos deben llamar a Split ya que es colectiva.
 * En este caso se le asigna a cada intracomunicador una variable 
 * diferente, para mostrar que es exactamente igual usar la misma o no, 
 * y se crean dos intracomunicadores en función del parámetro color. 
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv){
    
    int myrank, nprocs;
    int gproc;
    int rankparimpar;
    int color;
    MPI_Comm comPar, comImpar;
    char *namep;
    char *namei;
    int len = 0;

    // Inicializamos MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    namep = (char *)malloc(57*sizeof(char));
    namei = (char *)malloc(57*sizeof(char));
    
    // Con color determinamos a qué intracomunicador pertenece cada proceso
    color = myrank%2;
    // Los procesos pares le darán una variable a su comunicador y los impares otra
    // pero todos los procesos del comunicador origen, MPI_COMM_WORLD
    // deben llamar a split aunque cada uno le dé una variable a su comunicador.
    if (color==0){
	// Los pares crean su comunicador
	MPI_Comm_split(MPI_COMM_WORLD, color, myrank, &comPar);
	MPI_Comm_set_name(comPar,"comPar");
	// Cada proceso obtiene el tamaño de su intracomunicador y que rango
	// tiene dentro de él
	MPI_Comm_size(comPar, &gproc);
	MPI_Comm_rank(comPar, &rankparimpar);
	MPI_Comm_get_name(comPar,namep,&len);	
	printf("Soy %d, par, y dentro de mi nuevo comunicador (%s) de tamaño %d soy %d \n",myrank, namep, gproc, rankparimpar);
    }else{
	// Los impares crean su comunicador
	MPI_Comm_split(MPI_COMM_WORLD, color, myrank, &comImpar);
	MPI_Comm_set_name(comImpar,"comImpar");
	// Cada proceso obtiene el tamaño de su intracomunicador y que rango
	// tiene dentro de él
	MPI_Comm_size(comImpar, &gproc);
	MPI_Comm_rank(comImpar, &rankparimpar);
	MPI_Comm_get_name(comImpar,namei,&len);
	printf("Soy %d, impar, y dentro de mi nuevo comunicador (%s) de tamaño %d soy %d \n",myrank, namei, gproc, rankparimpar);
    }
    
    MPI_Finalize();
    return 0;
}
