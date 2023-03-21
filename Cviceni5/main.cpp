#include <cmath>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>

#include "Field.h"
#include "PoissonSolver.hpp"
#include "BoundaryCondition.hpp"
#include "DirichletBC.hpp"

int main() {

	double h = 0.001;
	PoissonSolver mySolver = PoissonSolver((int) 1.0/h, h);


	std::shared_ptr<BoundaryCondition> topBC = std::make_shared<DirichletBC>();
	std::shared_ptr<BoundaryCondition> bottomBC = std::make_shared<DirichletBC>();
	std::shared_ptr<BoundaryCondition> leftBC = std::make_shared<DirichletBC>();
	std::shared_ptr<BoundaryCondition> rightBC = std::make_shared<DirichletBC>();
	mySolver.setBoundaryCondition(topBC, bottomBC, leftBC, rightBC);

	mySolver.solve();


	Field<double> u = mySolver.getU();

	double num = u(480, 500);
	double num2 = u(500, 500);
	double num3 = u(300, 500);

    /*auto stop1 = std::chrono::high_resolution_clock::now();

	std::cout << std::fixed << f.meanParallel(8) << "\n";
	
	auto stop2 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - stop1).count() << " ms\n";*/


	return 0;
}