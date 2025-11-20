/* Asignatura: Procesamiento Paralelo
 * Grado: Tecnologías de la Información
 * Departamento: Informática y Automática, ETSI Informática, UNED
 * Autores: David Moreno Salinas, Victorino Sanz Prat
 * Nombre: Ejercicio2.c
 * Explicación:
 * Creación de N procesos de forma estática. Crearán un grupo de M hijos
 * de forma dinámica. Se realiza una comunicación punto a punto entre el 
 * proceso padre de rango 0 y el hijo de rango 0 para mostrar como deben
 * usar el intercomunicador. Igualmente una comunicación punto a punto entre
 * el 0 y el N-1 de los padres, y el 0 y el M.1 de los hijos, para mostrar
 * como tanto padres como hijos tienen su propio MPI_COMM_WORLD. 
 * Notas: reproduzca el código y modifíquelo para probar su funcionamiento
 * e introducir la variantes que considere oportunas. 
 * */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int my_world_rank;
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
		// Es un proceso estático por lo que pasa a la creación de dinámicos
		// Todos los estáticos ejecutan spawn al ser colectiva y 
		// establecemos un número cualquiera de procesos a crear
		mprocs = 10; 
		MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, mprocs, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercom, MPI_ERRCODES_IGNORE);
		// Nótese que en el lado de los padres el intercomunicador para 
		// comunicarse con los hijos es intercom
		if (my_world_rank==0) {
			// el proceso 0 de los padres envía un dato al proceso 0 de los hijos
			token = 50;
			MPI_Send(&token, 1, MPI_INT, 0, 99, intercom);
			// Y manda el token también al proceso N-1 de los padres
			token = 100;
			MPI_Send(&token, 1, MPI_INT, nprocs-1, 99, MPI_COMM_WORLD);
		} else if (my_world_rank==nprocs-1) {
			MPI_Recv(&token, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		printf("Soy el proceso padre %d, y el token que poseo es %d \n", my_world_rank, token);
	}else{
		// es un proceso creado dinámicamente, en este lado el nombre del
		// intercomunicador para comunicarse con los padres es "padre"
		// que fue el nombre que se le dió con la función get_parent
		// El hijo 0 recibe del padre 0 y lo manda al hijo M-1
		if (my_world_rank==0) {
			MPI_Recv(&token, 1, MPI_INT, 0, 99, padre, MPI_STATUS_IGNORE);
			MPI_Send(&token, 1, MPI_INT, nprocs-1, 99, MPI_COMM_WORLD);
		} else if (my_world_rank==nprocs-1){
			MPI_Recv(&token, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		}
		printf("Soy el proceso hijo %d, y el token que poseo es %d \n", my_world_rank, token);
	}
	MPI_Finalize();
    return 0;
}
