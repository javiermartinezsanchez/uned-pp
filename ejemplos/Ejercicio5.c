/* Asignatura: Procesamiento Paralelo Grado: Tecnologías de la
 * Información Departamento: Informática y Automática, ETSI
 * Informática, UNED Autores: David Moreno Salinas, Victorino Sanz
 * Prat Nombre: Ejercicio5.c Explicación: Creación de N procesos de
 * forma estática. El proceso N-1 creará un grupo de M hijos de forma
 * dinámica. Se crea un intracomunicador a partir del intercomunicador
 * entre padre e hijos. Se muestra también el rango que cada proceso
 * tiene en el nuevo intracomunicador.  Notas: reproduzca el código y
 * modifíquelo para probar su funcionamiento e introducir la variantes
 * que considere oportunas. */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int my_world_rank, my_intra_rank;
    int nprocs, mprocs;
    MPI_Comm padre, intercom, intracom;
    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    // Rango y tamaño del MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_world_rank);
    // Comprobamos si el proceso ha sido creado estática o dinámicamente
    MPI_Comm_get_parent(&padre);
    if (padre==MPI_COMM_NULL){ 
	// Es un proceso estático por lo que pasa a la creación de dinámicos
	// establecemos un número cualquiera de procesos a crear
	mprocs = 4; 
	if (my_world_rank==nprocs-1) { 
	    MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, mprocs, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
	    // Nótese que en el lado del padre el intercomunicador para 
	    // comunicarse con los hijos es intercom
	    // Hacemos el merge para incluir todos los procesos de intercom 
	    // en un único intracomunicador. Todos los procesos de intercom
	    // deben llamarlo. Como solo el N-1 de los estáticos está en intercom
	    // sólo debe llamarlo él.
	    MPI_Intercomm_merge(intercom, 0, &intracom);
	    // Rango del proceso en el nuevo intracomunicador
	    MPI_Comm_rank(intracom, &my_intra_rank);		
	    printf("Soy el proceso estático %d en mi MPI_COMM_WORLD y %d en intracom \n", my_world_rank, my_intra_rank);
	    //liberamos comunicador, pero sólo lo hacen los procesos que lo posean
	    MPI_Comm_free(&intracom);
	} else {
	    printf("Soy el proceso estático %d en mi MPI_COMM_WORLD y no tengo hijos \n", my_world_rank);
	}
    } else {
	// es un proceso creado dinámicamente, en este lado el nombre del
	// intercomunicador para comunicarse con el padre es "padre"
	// que fue el nombre que se le dió con la función get_parent
	// Hacemos el merge para incluir todos los procesos de intercom 
	// en un único intracomunicador. Todos los procesos del intercomunicador
	// deben llamarlo, en este lado el nombre del intercomunicador es padre
	MPI_Intercomm_merge(padre, 0, &intracom);
	// Rango del proceso en el nuevo intracomunicador
	MPI_Comm_rank(intracom, &my_intra_rank);
	printf("Soy el proceso dinámico %d en mi MPI_COMM_WORLD y %d en intracom \n", my_world_rank, my_intra_rank);
	//liberamos comunicador, pero sólo lo hacen los procesos que lo posean
	MPI_Comm_free(&intracom);
    }
    MPI_Finalize();
    return 0;
}
