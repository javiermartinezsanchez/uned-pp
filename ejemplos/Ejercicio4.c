/* Asignatura: Procesamiento Paralelo
 * Grado: Tecnologías de la Información
 * Departamento: Informática y Automática, ETSI Informática, UNED
 * Autores: David Moreno Salinas, Victorino Sanz Prat
 * Nombre: Ejercicio4.c
 * Explicación:
 * Creación de N procesos de forma estática. Crearán un grupo de M hijos
 * de forma dinámica. Se crea un intracomunicador a partir del intercomunicador
 * entre padres e hijos que permite la comunicación directa entre todos los 
 * padres y todos los hijos. Para probarlo se realiza un broadcast sobre
 * dicho intracomunicador. Se muestra también el rango que cada proceso
 * tiene en el nuevo intracomunicador. 
 * Notas: reproduzca el código y modifíquelo para probar su funcionamiento
 * e introducir la variantes que considere oportunas. 
 * */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv){
    int my_world_rank, my_intra_rank;
    int nprocs, mprocs;
    int token;
    MPI_Comm padre, intercom, intracom;
    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    // Rango y tamaño del MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_world_rank);
    // Comprobamos si el proceso ha sido creado estática o dinámicamente
    MPI_Comm_get_parent(&padre);
    token = 0;
    if (padre==MPI_COMM_NULL){ 
	// Es un proceso estático por lo que pasa a la creación de dinámicos
	// Todos los estáticos ejecutan spawn al ser colectiva y 
	// establecemos un número cualquiera de procesos a crear
	mprocs = 4; 
	MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, mprocs, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercom, MPI_ERRCODES_IGNORE);
	// Nótese que en el lado de los padres el intercomunicador para 
	// comunicarse con los hijos es intercom
	// HAcemos el merge para incluir todos los procesos de intercom 
	// en un único intracomunicador. Todos los procesos de intercom
	// deben llamarlo
	MPI_Intercomm_merge(intercom, 0, &intracom);
	// Rango del proceso en el nuevo intracomunicador
	MPI_Comm_rank(intracom, &my_intra_rank);
	if (my_world_rank==0) {
	    // el proceso 0 de los padres debe envíar un dato a todos
	    // Lo inicializa a 50
	    token = 50;
	} 
	printf("Soy el proceso padre %d en mi MPI_COMM_WORLD y %d en intracom \n", my_world_rank, my_intra_rank);
    }else{
	// es un proceso creado dinámicamente, en este lado el nombre del
	// intercomunicador para comunicarse con los padres es "padre"
	// que fue el nombre que se le dió con la función get_parent
	// Hacemos el merge para incluir todos los procesos de intercom 
	// en un único intracomunicador. Todos los procesos de intercom
	// deben llamarlo, en este lado intercom es padre
	MPI_Intercomm_merge(padre, 1, &intracom);
	// Rango del proceso en el nuevo intracomunicador
	MPI_Comm_rank(intracom, &my_intra_rank);
	printf("Soy el proceso hijo  %d en mi MPI_COMM_WORLD y %d en intracom \n", my_world_rank, my_intra_rank);
    }
    MPI_Bcast(&token, 1, MPI_INT, 0, intracom);
    printf("Soy el proceso %d en intracom, y el token que poseo es %d \n", my_intra_rank, token);
    MPI_Comm_free(&intracom);
    MPI_Finalize();
    return 0;
}
