/* Asignatura: Procesamiento Paralelo
 * Grado: Tecnologías de la Información
 * Departamento: Informática y Automática, ETSI Informática, UNED
 * Autores: David Moreno Salinas, Victorino Sanz Prat
 * Explicación:
 * Creación de N procesos de forma estática. Los procesos pares crearán un grupo de M hijos
 * de forma dinámica. Para ello en primer lugar se crea un intracomunicador
 * con los procesos pares, y a partir de este se crean los M hijos. 
 * Se crea un intracomunicador a partir del intercomunicador
 * entre padres e hijos. Se muestra también el rango que cada proceso
 * tiene en el nuevo intracomunicador. 
 * Notas: reproduzca el código y modifíquelo para probar su funcionamiento
 * e introducir la variantes que considere oportunas. 
 * */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int my_world_rank, my_intra_rank;
    int nprocs, mprocs;
    int color;
    MPI_Comm padre, intercom, intracom, intrapar;
	// Inicialización de MPI
    MPI_Init(&argc, &argv);
    // Rango y tamaño del MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_world_rank);
    // Comprobamos si el proceso ha sido creado estática o dinámicamente
    MPI_Comm_get_parent(&padre);
	if (padre==MPI_COMM_NULL){ // Estáticos
		// Diferenciamos entre pares e impares
		color = my_world_rank%2;
		if (color == 0) {
			// Es un proceso par por lo que pasa a la creación de dinámicos
			// Todos los estáticos paresejecutan spawn al ser colectiva y 
			// establecemos un número cualquiera de procesos a crear
			mprocs = 4; 
			MPI_Comm_split(MPI_COMM_WORLD, color, my_world_rank, &intrapar);
			MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, mprocs, MPI_INFO_NULL, 0, intrapar, &intercom, MPI_ERRCODES_IGNORE);
			// Nótese que en el lado de los padres el intercomunicador para 
			// comunicarse con los hijos es intercom
			// Hacemos el merge para incluir todos los procesos de intercom 
			// en un único intracomunicador. Todos los procesos de intercom
			// deben llamarlo
			MPI_Intercomm_merge(intercom, 0, &intracom);
			// Rango del proceso en el nuevo intracomunicador
			MPI_Comm_rank(intracom, &my_intra_rank);		
			printf("Soy el proceso estático %d en mi MPI_COMM_WORLD y %d en intracom \n", my_world_rank, my_intra_rank);
			//liberamos comunicadores, pero sólo lo hacen los procesos que los posean
			MPI_Comm_free(&intrapar);
			MPI_Comm_free(&intracom);
		} else {
			// Los procesos impares no crean ningún comunicador, pero 
			// deben llamar a Split igualmente ya que es colectiva
			// para todos los procesos del comunicador original
			MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, my_world_rank, &intrapar);
			printf("Soy el proceso estático %d en mi MPI_COMM_WORLD y no estoy en intracom \n", my_world_rank);
		} 
	}else{
		// es un proceso creado dinámicamente, en este lado el nombre del
		// intercomunicador para comunicarse con los padres es "padre"
		// que fue el nombre que se le dió con la función get_parent
		// Hacemos el merge para incluir todos los procesos del intercomunicador 
		// en un único intracomunicador. Todos los procesos del intercomunicador
		// deben llamarlo
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
