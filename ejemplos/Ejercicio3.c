/* Asignatura: Procesamiento Paralelo
 * Grado: Tecnologías de la Información
 * Departamento: Informática y Automática, ETSI Informática, UNED
 * Autores: David Moreno Salinas, Victorino Sanz Prat
 * Nombre: Ejercico3.c
 * Explicación:
 * Creación de N procesos de forma estática. El estático N-1 creará un grupo 
 * de M hijos de forma dinámica. Si un proceso estático diferente al padre, 
 * por ejemplo, el proceso de rango 1, quiere mandar un valor a los dinámicos,
 * debe hacerlo a través del padre, y él lo enviará a los hijos.
 * Notas: reproduzca el código y modifíquelo para probar su funcionamiento
 * e introducir la variantes que considere oportunas. 
 * */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv){
    int my_world_rank, my_inter_rank;
    int nprocs, mprocs;
    int token;
    MPI_Comm padre, intercom;
    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    // Rango y tamaño del MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_world_rank);
    // Comprobamos si el proceso ha sido creado estática o dinámicamente
    MPI_Comm_get_parent(&padre);
    token = 0;
    if (padre==MPI_COMM_NULL){ 
	// Es un proceso estático. Sólo el N-1 creará los hijos por lo que
	// sólo el proceso de rango 0 llama a spawn usando MPI_COMM_SELF
	mprocs = 10; 
	if (my_world_rank == nprocs-1) {
	    MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, mprocs, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
	    MPI_Comm_rank(intercom, &my_inter_rank);
	    printf("Soy el proceso padre con rango %d en MPI_COMM_WORLD, y rango %d en mi grupo del intercomunicador \n", my_world_rank, my_inter_rank);
	    // Nótese que en el lado del padre el intercomunicador para 
	    // comunicarse con los hijos es intercom
	    // el proceso padre recibe un dato del estático 1
	    MPI_Recv(&token, 1, MPI_INT, 1, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    // Y  envía el dato al proceso a todos los hijos.
	    MPI_Bcast(&token, 1, MPI_INT, MPI_ROOT, intercom);
	} else if (my_world_rank==1) {
	    token = 100;
	    MPI_Send(&token, 1, MPI_INT, nprocs-1, 99, MPI_COMM_WORLD);
	}
	printf("Soy el proceso estático %d, y el token que poseo es %d \n", my_world_rank, token);
    }else{
	// es un proceso creado dinámicamente, en este lado el nombre del
	// intercomunicador para comunicarse con el padre es "padre"
	// que fue el nombre que se le dió con la función get_parent
	MPI_Comm_rank(padre, &my_inter_rank);
	// ETodos los hijos reciben con un broadcast a traveés del intercomunicador
	MPI_Bcast(&token, 1, MPI_INT, 0, padre);
	printf("Soy el proceso hijo %d en mi MPI_COMM_WORLD y %d en mi grupo del intercomunicador con mi padre, y el token que poseo es %d \n", my_world_rank, my_inter_rank, token);
    }
    MPI_Finalize();
    return 0;
}
