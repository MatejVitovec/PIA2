#include <mpi.h>
#include <iostream>

int main(int argc, char **argv) {

    MPI_Init(&argc,&argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	std::cout << "Procesor " << rank << "/" << size << "\n";

	int target;
	if (rank == 0) target = 1;
	else target = 0;

	if (rank == 0) {
		int num[2000];
		num[0] = 2;
		MPI_Send(&num, 2000, MPI_INT, target, 0, MPI_COMM_WORLD);
		int num2[2000];
		MPI_Recv(&num2, 2000, MPI_INT, target, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << num2[0] << " Proc: "<< rank << "\n";
	}
	else {
		int num2[2000];
		MPI_Recv(&num2, 2000, MPI_INT, target, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << num2[0] << "Proc: "<< rank << "\n";

		int num[2000];
		MPI_Send(&num, 2000, MPI_INT, target, 0, MPI_COMM_WORLD);
	}


    MPI_Finalize();

	system("sleep 2");
	
	return 0;
}