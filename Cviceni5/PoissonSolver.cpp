#include <cmath>

#include "PoissonSolver.hpp"
#include <iostream>
#include <fstream>

void PoissonSolver::setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right)
{
	topBC = top;
	bottomBC = bottom;
	leftBC = left;
	rightBC = right;
}


double PoissonSolver::f(int i, int j, double h)
{
	if((pow(i*h - 0.5, 2.0) + pow(j*h - 0.5, 2.0)) < 0.04)
	{
		return 50.0;
	}
	else
	{
		return 0.0;
	}
}

void PoissonSolver::applyBoundaryCondition()
{
	//todo podminky souhlasu

	for (int i = 0; i < n; i++)
	{
		u(i, 0) = bottomBC->apply();
	}

	for (int i = 0; i < n; i++)
	{
		u(i, n-1) = topBC->apply();
	}

	for (int i = 0; i < n; i++)
	{
		u(0, i) = leftBC->apply();
	}

	for (int i = 0; i < n; i++)
	{
		u(n-1, i) = rightBC->apply();
	}
	un = u;	
}

void PoissonSolver::solve()
{
	int iter = 10;

	applyBoundaryCondition();

	for (int ii = 0; ii < iter; ii++)
	{
		for (int j = 1; j < n-1; j++)
		{
			for (int i = 1; i < n-1; i++)
			{
				un(i, j) = (1.0/4.0)*(h*h*f(i, j, h) + u(i, j-1) + u(i-1, j) + u(i, j+1) + u(i+1, j));
			}			
		}
		u = un;		
	}
}

Field<double> PoissonSolver::getU()
{
	return u;
}

void PoissonSolver::saveData(std::string outputFileName)
{
    std::ofstream writeToFile(outputFileName);
    writeToFile << h << std::endl;
    writeToFile << n << std::endl;

    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
		{
			writeToFile << u(i, j) << std::endl;
		}
		
    }
        
    writeToFile.close();
}