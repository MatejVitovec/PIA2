#include <cmath>
#include <iostream>
#include <fstream>

#include "PoissonSolver.hpp"

PoissonSolver::PoissonSolver()
{
	h = 0.01;
	firstX = 0;
	firstY = 0;
	n = (int)(1.0/h);
	m = (int)(1.0/h);
	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;
}

PoissonSolver::PoissonSolver(double h_)
{
	h = h_;
	firstX = 0;
	firstY = 0;
	n = (int)(1.0/h);
	m = (int)(1.0/h);
	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;
}

/*PoissonSolver::PoissonSolver(double h, double intX, double intY)
{
	h = h;
	firstX = 0;
	firstY = 0;
	n = (int)(intX/h);
	m = (int)(intY/h);
	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;
}*/

void PoissonSolver::setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right)
{
	//MPI TODO

	topBC = top;
	bottomBC = bottom;
	leftBC = left;
	rightBC = right;
}

void PoissonSolver::setTargetError(double err)
{
	targetError = err;
}

void PoissonSolver::setFunctionValues(Field<double> f_)
{
	//MPI TODO

	func = f_;
}

void PoissonSolver::setFunctionValues(double (*function_ptr)(int, int, double))
{
	//MPI TODO

	for (int j = 0; j < m; j++)
	{
		for (int i = 0; i < n; i++)
		{
			func(i, j) = function_ptr(i, j, h);
		}
	}
}

void PoissonSolver::solveBoundaryCondition(Field<double>& un)
{
	//MPI TODO

	int nMinusOne = n - 1;
	int nMinusTwo = n - 2;
	int mMinusOne = m - 1;
	int mMinusTwo = m - 2;
	double hh = h;

	//bottom
	for (int i = 1; i < nMinusOne; i++)
	{
		un(i, 0) = bottomBC->apply(u(i-1, 0), u(i, 1), u(i+1, 0), func(i, 0), hh);
	}

	//top
	for (int i = 1; i < nMinusOne; i++)
	{
		un(i, nMinusOne) = topBC->apply(u(i+1, nMinusOne), u(i, nMinusTwo), u(i-1, nMinusOne), func(i, nMinusOne), hh);
	}

	//left
	for (int j = 1; j < mMinusOne; j++)
	{
		un(0, j) = leftBC->apply(u(0, j+1), u(1, j), u(0, j-1), func(0, j), hh);
	}

	//right
	for (int j = 1; j < mMinusOne; j++)
	{
		un(mMinusOne, j) = rightBC->apply(u(mMinusOne, j-1), u(mMinusTwo, j), u(mMinusOne, j+1), func(mMinusOne, j), hh);
	}

	//corners
	un(0, 0) = (u(1, 0) + u(0, 1))/2.0;
	un(nMinusOne, 0) = (u(nMinusTwo, 0) + u(nMinusOne, 1))/2.0;
	un(0, mMinusOne) = (u(1, mMinusOne) + u(0, mMinusTwo))/2.0;
	un(nMinusOne, mMinusOne) = (u(nMinusTwo, mMinusOne) + u(nMinusOne, mMinusTwo))/2.0;

}

void PoissonSolver::solve()
{
	//Jacobi	
	Field<double> un(n, m);

	int iter = 0;
	double error = 10e10;
	int nn = n - 1;
	int mm = m - 1;

	solveBoundaryCondition(un);
	u = un;

	while (error > targetError)
	{
		++iter;

		for (int j = 1; j < mm; ++j)
		{
			for (int i = 1; i < nn; ++i)
			{
				un(i, j) = (1.0/4.0)*(h*h*func(i, j) + u(i, j-1) + u(i-1, j) + u(i, j+1) + u(i+1, j));
			}			
		}

		solveBoundaryCondition(un);

		error = calculateError(un);

		u = un;
	}
	std::cout << "vypocet probehl s " << iter << " iteracemi" << std::endl; 
}

double PoissonSolver::calculateError(const Field<double>& un)
{
	//MPI TODO

	Field<double> aux = un - u;

	return aux.normEuclid();
}

Field<double> PoissonSolver::getResult()
{
	//MPI TODO

	return u;
}

void PoissonSolver::saveData(std::string outputFileName)
{
    std::ofstream writeToFile(outputFileName);
    writeToFile << h << std::endl;
    writeToFile << n << std::endl;
	writeToFile << m << std::endl;

    for (int j = 0; j < m; j++)
    {
        for (int i = 0; i < n; i++)
		{
			writeToFile << u(i, j) << std::endl;
		}		
    }
        
    writeToFile.close();
}