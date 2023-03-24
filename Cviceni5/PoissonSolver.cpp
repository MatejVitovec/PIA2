#include <cmath>
#include <omp.h>

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

void PoissonSolver::setTargetError(double err)
{
	targetError = err;
}

void PoissonSolver::setInterval(double interval_)
{
	interval = interval_;
	n = (int) interval/h;

	u = Field<double>(n ,n);
}

void PoissonSolver::setStep(double hh)
{
	h = hh;
	n = (int) interval/h;

	u = Field<double>(n ,n);
}

void PoissonSolver::setFunctionValues(Field<double> f_)
{
	func = f_;
}

 void PoissonSolver::setFunctionValues(double (*function_ptr)(int, int, double))
 {
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < n; i++)
		{
			func(i, j) = function_ptr(i, j, h);
		}		
	}
 }

void PoissonSolver::solveBoundaryCondition(Field<double>& un)
{
	//todo podminky souhlasu
	int nMinusOne = n - 1;
	int nMinusTwo = n - 2;
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
	for (int j = 1; j < nMinusOne; j++)
	{
		un(0, j) = leftBC->apply(u(0, j+1), u(1, j), u(0, j-1), func(0, j), hh);
	}

	//right
	for (int j = 1; j < nMinusOne; j++)
	{
		un(nMinusOne, j) = rightBC->apply(u(nMinusOne, j-1), u(nMinusTwo, j), u(nMinusOne, j+1), func(nMinusOne, j), hh);
	}

	//corners
	un(0, 0) = (u(1, 0) + u(0, 1))/2.0;
	un(nMinusOne, 0) = (u(nMinusTwo, 0) + u(nMinusOne, 1))/2.0;
	un(0, nMinusOne) = (u(1, nMinusOne) + u(0, nMinusTwo))/2.0;
	un(nMinusOne, nMinusOne) = (u(nMinusTwo, nMinusOne) + u(nMinusOne, nMinusTwo))/2.0;

}

void PoissonSolver::solveBoundaryConditionRedBlack(Field<double>& u, Field<double>& un, int redBlack)
{
	//todo podminky souhlasu
	int nMinusOne = n - 1;
	int nMinusTwo = n - 2;
	double hh = h;

	//bottom
	for (int i = 2 - redBlack; i < nMinusOne; i = i + 2)
	{
		un(i, 0) = bottomBC->apply(u(i-1, 0), u(i, 1), u(i+1, 0), func(i, 0), hh);
	}

	//left
	for (int j = 2 - redBlack; j < nMinusOne; j = j + 2)
	{
		un(0, j) = leftBC->apply(u(0, j+1), u(1, j), u(0, j-1), func(0, j), hh);
	}

	if(!(n % 2 == 0))
	{
		//top
		for (int i = 2 - redBlack; i < nMinusOne; i = i + 2)
		{
			un(i, nMinusOne) = topBC->apply(u(i+1, nMinusOne), u(i, nMinusTwo), u(i-1, nMinusOne), func(i, nMinusOne), hh);
		}

		//right
		for (int j = 2 - redBlack; j < nMinusOne; j = j + 2)
		{
			un(nMinusOne, j) = rightBC->apply(u(nMinusOne, j-1), u(nMinusTwo, j), u(nMinusOne, j+1), func(nMinusOne, j), hh);
		}
	}
	else
	{
		//top
		for (int i = 1 + redBlack; i < nMinusOne; i = i + 2)
		{
			un(i, nMinusOne) = topBC->apply(u(i+1, nMinusOne), u(i, nMinusTwo), u(i-1, nMinusOne), func(i, nMinusOne), hh);
		}

		//right
		for (int j = 1 + redBlack; j < nMinusOne; j = j + 2)
		{
			un(nMinusOne, j) = rightBC->apply(u(nMinusOne, j-1), u(nMinusTwo, j), u(nMinusOne, j+1), func(nMinusOne, j), hh);
		}
	}

	//corners
	if(redBlack == 0)
	{
		//red
		un(0, 0) = (u(1, 0) + u(0, 1))/2.0;
		un(nMinusOne, nMinusOne) = (u(nMinusTwo, nMinusOne) + u(nMinusOne, nMinusTwo))/2.0;

		if(!(n % 2 == 0))
		{
			un(nMinusOne, 0) = (u(nMinusTwo, 0) + u(nMinusOne, 1))/2.0;
			un(0, nMinusOne) = (u(1, nMinusOne) + u(0, nMinusTwo))/2.0;
		}
		
	}
	else
	{
		if(n % 2 == 0)
		{
			un(nMinusOne, 0) = (u(nMinusTwo, 0) + u(nMinusOne, 1))/2.0;
			un(0, nMinusOne) = (u(1, nMinusOne) + u(0, nMinusTwo))/2.0;
		}
	}
}

void PoissonSolver::solve()
{
	//Jacobi	
	Field<double> un(n, n);

	int iter = 0;
	double error = 10e10;
	int nn = n - 1;

	solveBoundaryCondition(un);
	u = un;

	while (error > targetError)
	{
		++iter;

		//#pragma omp parallel for collapse(2)
		for (int j = 1; j < nn; ++j)
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

void PoissonSolver::solveGaussSeide()
{
	//GaussSeidel - red black algorithm

	int iter = 0;
	double error = 10e10;
	int nn = n - 1;	

	solveBoundaryCondition(u);
	
	Field<double> uRed = u;
	Field<double> uBlack = u;

	while (error > targetError)
	{
		++iter;

		//red nodes 0,0 ; 1,1
		//#pragma omp parallel for collapse(2)
		for (int j = 1; j < nn; j = j + 2)
		{
			for (int i = 1; i < nn; i = i + 2)
			{
				uRed(i, j) = (1.0/4.0)*(h*h*func(i, j) + uBlack(i, j-1) + uBlack(i-1, j) + uBlack(i, j+1) + uBlack(i+1, j));				
			}			
		}

		for (int j = 2; j < nn; j = j + 2)
		{
			for (int i = 2; i < nn; i = i + 2)
			{
				uRed(i, j) = (1.0/4.0)*(h*h*func(i, j) + uBlack(i, j-1) + uBlack(i-1, j) + uBlack(i, j+1) + uBlack(i+1, j));				
			}			
		}

		solveBoundaryConditionRedBlack(uBlack, uRed, 0);

		//black nodes 0,1 ; 1,0
		//#pragma omp parallel for collapse(2)
		for (int j = 1; j < nn; j = j + 2)
		{
			for (int i = 2; i < nn; i = i + 2)
			{
				uBlack(i, j) = (1.0/4.0)*(h*h*func(i, j) + uRed(i, j-1) + uRed(i-1, j) + uRed(i, j+1) + uRed(i+1, j));				
			}			
		}
		for (int j = 2; j < nn; j = j + 2)
		{
			for (int i = 1; i < nn; i = i + 2)
			{
				uBlack(i, j) = (1.0/4.0)*(h*h*func(i, j) + uRed(i, j-1) + uRed(i-1, j) + uRed(i, j+1) + uRed(i+1, j));				
			}			
		}

		solveBoundaryConditionRedBlack(uRed, uBlack, 1);

		Field<double> un = uRed + uBlack;
		error = calculateError(un);
		u = un;
	}

	std::cout << "vypocet probehl s " << iter << " ietracemi" << std::endl; 
}



double PoissonSolver::calculateError(const Field<double>& un)
{
	Field<double> aux = un - u;

	return aux.normEuclid();
}


Field<double> PoissonSolver::getResult()
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