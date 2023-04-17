#include <cmath>
#include <memory>
#include <iostream>
#include <mpi.h>

#include "PoissonSolver.hpp"

#include "DirichletBC.hpp"
#include "NeumannBC.hpp"

double poissonFunction(int i, int j, double hh)
{
	if((pow(i*hh - 0.5, 2.0) + pow(j*hh - 0.5, 2.0)) < 0.04)
	{
		return 50.0;
	}
	else
	{
		return 0.0;
	}
}


int main(int argc, char **argv) {

	//MPI INIT
    MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	PoissonSolver mySolver = PoissonSolver(rank, size, 0.01);

	mySolver.setFunctionValues(poissonFunction);

	//top, bottom, left, right
	mySolver.setBoundaryCondition(std::make_shared<NeumannBC>(0.0, BoundaryCondition::TOP),
									std::make_shared<DirichletBC>(0.0, BoundaryCondition::BOTTOM),
									std::make_shared<DirichletBC>(0.0, BoundaryCondition::LEFT),
									std::make_shared<DirichletBC>(3.0, BoundaryCondition::RIGHT));

	mySolver.solve();

	mySolver.saveData("data.txt");

	MPI_Finalize();

	return 0;
}