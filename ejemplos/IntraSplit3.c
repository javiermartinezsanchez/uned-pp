/******************************************************************************
 * Fichero: IntraSplit3.c
 *
 * Compilar con:    mpicc IntraSplit3.c -o split3
 * Ejecutar con:    mpirun -np X split3
 * siendo X un número entero mayor que 1. Preferiblemente impar para ver
 * la diferencia en el número de procesos por intracomunicador
 
 * Creamos de forma estática un conjunto de procesos, y se crearán
 * dos intracomunicadores directamente con MPI_Comm_split, pero en este caso
 * los procesos podrán pertenecer a los dos, a uno o a ningún comunicador. 
 * Se creará uno para procesos pares y otro para múltiplos de 3.
 * Todos los procesos deben llamar a Split ya que es colectiva. En este caso 
 * se deben hacer dos llamadas a split por cada proceso, ya que los comunicadores
 * no son disjuntos. Aquel proceso que no esté en un determinado comunicador
 * pasará en el parámetro color el valor MPI_UNDEFINED en split. 
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv){
    
    int myrank, nprocs;
    int gproc;
    int rankcomm;
    int color, color3;
    MPI_Comm comPar, comMult3;
    char *namep;
    char *namem;
    int len = 0;
    
    // Inicializamos MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    namem = (char *)malloc(57*sizeof(char));
    namep = (char *)malloc(57*sizeof(char));
    
    // Creamos el primer comunicador para los procesos pares, y asignamos
    // a color el mismo valor para todos los procesos pares.
    color = myrank%2;
    if (color==0){
	// Si el proceso es par, crea su comunicador de procesos pares
	MPI_Comm_split(MPI_COMM_WORLD, color, myrank, &comPar);
	MPI_Comm_set_name(comPar,"compar");
	// Cada proceso para obtiene el tamaño de su intracomunicador y que rango
	// tiene dentro de él
	MPI_Comm_size(comPar, &gproc);
	MPI_Comm_rank(comPar, &rankcomm);
	MPI_Comm_get_name(comPar,namep,&len);
	printf("Soy %d, par, y dentro de mi nuevo comunicador (%s) de tamaño %d soy %d \n",myrank, namep, gproc, rankcomm);
    }else{
	// Los impares también deben llamar a split pero no crean comunicador
	// para ello en color se pasa MPI_UNDEFINED
	MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, myrank, &comPar);
    }
    // Ahora se crea un intracomunicador para los múltiplos de 3
    color3 = myrank%3;
    if (color3==0){
	// Los múltiplos de 3 crean su comunicador
	MPI_Comm_split(MPI_COMM_WORLD, color3, myrank, &comMult3);
	MPI_Comm_set_name(comMult3,"mult3");
	// Cada proceso para obtiene el tamaño de su intracomunicador y que rango
	// tiene dentro de él
	MPI_Comm_size(comMult3, &gproc);
	MPI_Comm_rank(comMult3, &rankcomm);
	MPI_Comm_get_name(comMult3,namem,&len);
	printf("Soy %d, multiplo de 3, y dentro de mi nuevo comunicador (%s) de tamaño %d soy %d \n",myrank, namem, gproc, rankcomm);
    } else{
	// El resto de procesos llama a split con MPI_UNDEFINED en color
	// ya que no van a crear comunicador
	MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, myrank, &comMult3);
    }
    
    MPI_Finalize();
    return 0;
}
