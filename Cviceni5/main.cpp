#include <cmath>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <omp.h>

#include "Field.hpp"
#include "PoissonSolver.hpp"
#include "BoundaryCondition.hpp"
#include "DirichletBC.hpp"
#include "NeumannBC.hpp"

double poissonFunction(int i, int j, double hh)
{
	if((pow(i*hh - 0.5, 2.0) + pow(j*hh - 0.5, 2.0)) < 0.04)
	{
		return 50.0;
		//return 0.0;
	}
	else
	{
		return 0.0;
	}
}


int main() {

	//omp_set_num_threads(8);

	PoissonSolver mySolver = PoissonSolver(0.005);

	mySolver.setFunctionValues(poissonFunction);

	//top, bottom, left, right
	mySolver.setBoundaryCondition(std::make_shared<NeumannBC>(0.0),
									std::make_shared<DirichletBC>(0.0),
									std::make_shared<DirichletBC>(0.0),
									std::make_shared<DirichletBC>(2.0));

	auto stop1 = std::chrono::high_resolution_clock::now();

	mySolver.solveGaussSeidel();
	//mySolver.solve();

	auto stop2 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - stop1).count() << " ms\n";

	mySolver.saveData("data.txt");

	return 0;
}