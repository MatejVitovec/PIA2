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

int main() {

	omp_set_num_threads(8);

	double h = 0.01;
	PoissonSolver mySolver = PoissonSolver((int) 1.0/h, h);

	std::shared_ptr<BoundaryCondition> topBC = std::make_shared<DirichletBC>(0.0);
	std::shared_ptr<BoundaryCondition> bottomBC = std::make_shared<DirichletBC>(0.0);
	std::shared_ptr<BoundaryCondition> leftBC = std::make_shared<DirichletBC>(0.0);
	std::shared_ptr<BoundaryCondition> rightBC = std::make_shared<DirichletBC>(0.0);
	mySolver.setBoundaryCondition(topBC, bottomBC, leftBC, rightBC);

	//auto stop1 = std::chrono::high_resolution_clock::now();

	mySolver.solve();

	//auto stop2 = std::chrono::high_resolution_clock::now();
	//std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - stop1).count() << " ms\n";

	mySolver.saveData("data.txt");


	///Field<double> u = mySolver.getU();


    /*auto stop1 = std::chrono::high_resolution_clock::now();

	std::cout << std::fixed << f.meanParallel(8) << "\n";
	
	auto stop2 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - stop1).count() << " ms\n";*/


	return 0;
}