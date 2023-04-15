#include <cmath>
#include <iostream>
#include <fstream>

#include "PoissonSolver.hpp"
#include "VirtualBC.hpp"

PoissonSolver::PoissonSolver()
{
	h = 0.01;
	n = (int)(1.0/h);
	m = (int)(1.0/h);
	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;
}

PoissonSolver::PoissonSolver(int rank_, int size_, double h_)
{
	rank = rank_;
	size = size_;

	h = h_;

	int globalN = (int)(1.0/h);
	int globalM = (int)(1.0/h);

	bool perfectSquare = false;
	for (int i = 1; i * i <= size; i++)
	{
		if ((size % i == 0) && (size / i == i)) {
			perfectSquare = true;
		}
    }

	if(perfectSquare)
	{
		mpiMaxIndexX = sqrt(size);
		mpiMaxIndexY = mpiMaxIndexX;

		n = globalN/mpiMaxIndexX;
		m = globalM/mpiMaxIndexX;
	}
	else
	{
		mpiMaxIndexY = sqrt(size/2);
		mpiMaxIndexX = mpiMaxIndexY*2;

		n = globalN/mpiMaxIndexX;
		m = globalM/mpiMaxIndexY;
	}

	mpiIndexX = rank % mpiMaxIndexX;
	mpiIndexY = (rank - mpiIndexX)/mpiMaxIndexX;

	if(mpiIndexX != 0) n++;
	if(mpiIndexX != mpiMaxIndexX) n++;
	if(mpiIndexY != 0) m++;
	if(mpiIndexY != mpiMaxIndexY) m++;

	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;

	std::cout << "rank: " << rank << " mpiX: " << mpiIndexX << " mpiY: " << mpiIndexY << std::endl;
}

int PoissonSolver::collumsOfOptimalStructure(int size_)
{
	int colEstimate;
	int aux;

	while(colEstimate > 0)
	{
		colEstimate = round(sqrt(size_));
		aux = size_ / colEstimate;
		if(colEstimate*aux == size_) return colEstimate;
	}
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
	if(mpiIndexX == 0)
	{
		leftBC = left;
	}
	else
	{
		leftBC = std::make_shared<VirtualBC>(rank, rank - 1, BoundaryCondition::LEFT);
	}

	if(mpiIndexY == 0)
	{
		bottomBC = bottom;
	}
	else
	{
		bottomBC = std::make_shared<VirtualBC>(rank, rank - mpiMaxIndexX, BoundaryCondition::BOTTOM);
	}

	if(mpiIndexX == mpiMaxIndexX)
	{
		rightBC = right;
	}
	else
	{
		rightBC = std::make_shared<VirtualBC>(rank, rank + 1, BoundaryCondition::RIGHT);
	}

	if(mpiIndexY == mpiMaxIndexY)
	{
		topBC = top;
	}
	else
	{
		topBC = std::make_shared<VirtualBC>(rank, rank + mpiMaxIndexX, BoundaryCondition::TOP);
	}

}

void PoissonSolver::setTargetError(double err)
{
	targetError = err;
}

/*void PoissonSolver::setFunctionValues(Field<double> f_)
{
	func = f_;
}*/

void PoissonSolver::setFunctionValues(double (*function_ptr)(int, int, double))
{
	int nn = n;
	int mm = m;

	if(mpiIndexX != 0) nn--;
	if(mpiIndexY != 0) mm--;
	if(mpiIndexX != mpiMaxIndexX) nn--;
	if(mpiIndexY != mpiMaxIndexY) mm--;

	int startX = mpiIndexX*nn;
	int startY = mpiIndexY*mm;

	if(mpiIndexX != 0) startX--;
	if(mpiIndexY != 0) startY--;


	for (int j = 0; j < m; j++)
	{
		for (int i = 0; i < n; i++)
		{
			func(i, j) = function_ptr(startX + i, startY + j, h);
		}
	}
}



void PoissonSolver::solveBoundaryCondition(Field<double>& un)
{
	if(mpiIndexX % 2 == 0)
	{
		rightBC->apply(un, u, func, h);
		leftBC->apply(un, u, func, h);
	}
	else
	{
		leftBC->apply(un, u, func, h);
		rightBC->apply(un, u, func, h);
	}

	if(mpiIndexY % 2 == 0)
	{
		topBC->apply(un, u, func, h);
		bottomBC->apply(un, u, func, h);
	}
	else
	{
		bottomBC->apply(un, u, func, h);
		topBC->apply(un, u, func, h);
	}

	//corners of real boundary
	if(mpiIndexX == 0 && mpiIndexY == 0) {un(0, 0) = (u(1, 0) + u(0, 1))/2.0;} 
	if(mpiIndexX == mpiMaxIndexX && mpiIndexY == 0) {un(n-1, 0) = (u(n-2, 0) + u(n-1, 1))/2.0;}
	if(mpiIndexX == 0 && mpiIndexY == mpiMaxIndexY) {un(0, m-1) = (u(1, m-1) + u(0, m-2))/2.0;}
	if(mpiIndexX == mpiMaxIndexX && mpiIndexY == mpiMaxIndexY) {un(n-1, m-1) = (u(n-1, m-1) + u(n-1, m-2))/2.0;}
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

	while (/*error > targetError*/ iter < 1000)
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

		//error = calculateError(un);

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