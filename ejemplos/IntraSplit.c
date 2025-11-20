/******************************************************************************
 * Fichero: IntraSplit.c
 *
 * Compilar con:    mpicc IntraSplit.c -o split1
 * Ejecutar con:    mpirun -np X split1
 * siendo X un número entero mayor que 1. Preferiblemente impar para ver
 * la diferencia en el número de procesos por intracomunicador

 * Creamos de forma estática un conjunto de procesos, y se crearán
 * dos intracomunicadores directamente con MPI_Comm_split. Todos los 
 * procesos deben llamar a Split ya que es colectiva, y para simplificar 
 * el código se ha utilizado la misma variable para los dos 
 * intracomunicadores, aunque son dos diferentes comunicadores, 
 * uno para pares y otro para impares. 
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv){
    
    int myrank, nprocs;
    int gproc;
    int rankparimpar;
    int color;
    MPI_Comm comParImpar;
    char *name;
    int len = 0;
    
    // Inicializamos MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    name = (char *)malloc(57*sizeof(char));
     
    // con color determinamos a qué comunicador pertenecerá cada proceso
    color = myrank%2;
    // Dividimos el MPI_COMM_WORLD en dos intracomunicadores, ambos con la misma variable
    MPI_Comm_split(MPI_COMM_WORLD, color, myrank, &comParImpar);
    if (myrank%2 == 0)
	MPI_Comm_set_name(comParImpar,"comPar");
    else
	MPI_Comm_set_name(comParImpar,"comImpar");
    // Cada proceso obtiene el tamaño de su intracomunicador y que rango
    // tiene dentro de él
    MPI_Comm_size(comParImpar, &gproc);
    MPI_Comm_rank(comParImpar, &rankparimpar);
    MPI_Comm_get_name(comParImpar,name,&len);	
    printf("Soy %d, y dentro de mi nuevo comunicador (%s) de tamaño %d soy %d \n",myrank, name, gproc, rankparimpar);
    
    MPI_Finalize();
    return 0;
}
