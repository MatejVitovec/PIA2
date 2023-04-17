#include <cmath>
#include <iostream>
#include <fstream>

#include "PoissonSolver.hpp"
#include "VirtualBC.hpp"

void PoissonSolver::test()
{
	
}

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

	mpiN = colsOfOptimalStructure(size);
	mpiM = size/mpiN;

	n = globalN/mpiN;
	m = globalM/mpiM;

	mpiIndexX = rank % mpiN;
	mpiIndexY = (rank - mpiIndexX)/mpiN;

	if(mpiIndexX != 0) n++;
	if(mpiIndexX != (mpiN - 1)) n++;
	if(mpiIndexY != 0) m++;
	if(mpiIndexY != (mpiM - 1)) m++;

	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;

	std::cout << "rank: " << rank << " mpiX: " << mpiIndexX << " mpiY: " << mpiIndexY << " n:" << n << " m: " << m << std::endl;
}

int PoissonSolver::colsOfOptimalStructure(int size_)
{
	int colEstimate = round(sqrt(size_));
	int aux;

	while(colEstimate > 0)
	{
		aux = size_ / colEstimate;
		if(colEstimate*aux == size_) return colEstimate;
		colEstimate--;
	}

	return 1;
}

void PoissonSolver::setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right)
{
	if(mpiIndexX == 0)
	{
		leftBC = left;
		std::cout << "rank: " << rank << "byla pridana REALNA OP LEFT" << std::endl;
	}
	else
	{
		leftBC = std::make_shared<VirtualBC>(rank, rank - 1, BoundaryCondition::LEFT);
		std::cout << "rank: " << rank << "byla pridana OP LEFT, soused: " << rank - 1 << std::endl;
	}

	if(mpiIndexY == 0)
	{
		bottomBC = bottom;
		std::cout << "rank: " << rank << "byla pridana REALNA OP BOTTOM" << std::endl;
	}
	else
	{
		bottomBC = std::make_shared<VirtualBC>(rank, rank - mpiN, BoundaryCondition::BOTTOM);
		std::cout << "rank: " << rank << "byla pridana OP BOTTOM, soused: " << rank - mpiN << std::endl;
	}

	if(mpiIndexX == (mpiN - 1))
	{
		rightBC = right;
		std::cout << "rank: " << rank << "byla pridana REALNA OP RIGHT" << std::endl;
	}
	else
	{
		rightBC = std::make_shared<VirtualBC>(rank, rank + 1, BoundaryCondition::RIGHT);
		std::cout << "rank: " << rank << "byla pridana OP RIGHT, soused: " << rank + 1 << std::endl;
	}

	if(mpiIndexY == (mpiM - 1))
	{
		topBC = top;
		std::cout << "rank: " << rank << "byla pridana REALNA OP TOP" << std::endl;
	}
	else
	{
		topBC = std::make_shared<VirtualBC>(rank, rank + mpiN, BoundaryCondition::TOP);
		std::cout << "rank: " << rank << "byla pridana OP TOP, soused: " << rank + mpiN << std::endl;
	}
}

void PoissonSolver::setTargetError(double err)
{
	targetError = err;
}

void PoissonSolver::setFunctionValues(double (*function_ptr)(int, int, double))
{
	int nn = n;
	int mm = m;

	if(mpiIndexX != 0) nn--;
	if(mpiIndexY != 0) mm--;
	if(mpiIndexX != (mpiN - 1)) nn--;
	if(mpiIndexY != (mpiM - 1)) mm--;

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

	//std::cout << "rank: " << rank << " startX: " << startX << " startY: " << startY << " n: "<< n << " m: " << m << std::endl;
}


void PoissonSolver::solveBoundaryCondition(Field<double>& un)
{
	//std::cout << "rank: " << rank << " bc start" << std::endl;
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
	if(mpiIndexX == 0 && mpiIndexY == 0){ un(0, 0) = (u(1, 0) + u(0, 1))/2.0;}
	if(mpiIndexX == (mpiN - 1) && mpiIndexY == 0) un(n-1, 0) = (u(n-2, 0) + u(n-1, 1))/2.0;
	if(mpiIndexX == 0 && mpiIndexY == (mpiM - 1)) un(0, m-1) = (u(1, m-1) + u(0, m-2))/2.0;
	if(mpiIndexX == (mpiN - 1) && mpiIndexY == (mpiM - 1)) un(n-1, m-1) = (u(n-1, m-1) + u(n-1, m-2))/2.0;
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

	while (iter < 10000)
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

	std::cout << "rank: " << rank << " ,vypocet probehl s " << iter << " iteracemi" << std::endl; 
}

/*double PoissonSolver::calculateError(const Field<double>& un)
{
	//MPI TODO

	Field<double> aux = un - u;

	return aux.normEuclid();
}*/

Field<double> PoissonSolver::getResult()
{
	if(rank == 0)
	{
		int nn = n;
		int mm = m;

		if(mpiN > 1) nn = n - 1;
		if(mpiM > 1) mm = m - 1;

		Field<double> result(nn * mpiN, mm * mpiM);

		for (int j = 0; j < m; j++)
		{
			for (int i = 0; i < n; i++)
			{
				result(i, j) = u(i, j);
			}				
		}

		for (int iRank = 1; iRank < size; iRank++)
		{
			int startIndexX = (iRank % mpiN) * nn;
			int startIndexY = (iRank / mpiN) * mm;

			std::vector<double> recieveData(nn*mm);

			std::cout << "recieve data n: " << nn << "recieve data m: " << mm << " startX: " << startIndexX << " startY: " << startIndexY << std::endl;
			MPI_Recv(&recieveData[0], nn*mm, MPI_DOUBLE, iRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			for (int j = 0; j < mm; j++)
			{
				for (int i = 0; i < nn; i++)
				{
					result(i + startIndexX, j + startIndexY) = recieveData[i + j*nn];
				}				
			}
		}

		return result;
	}
	else
	{
		int startI = 0;
		int startJ = 0;
		int endI = n;
		int endJ = m;

		if(mpiIndexX != 0) startI++;
		if(mpiIndexX != (mpiN - 1)) endI--;
		if(mpiIndexY != 0) startJ++;
		if(mpiIndexY != (mpiM - 1)) endJ--;

		int sendN = endI - startI;
		int sendM = endJ - startJ;

		std::vector<double> sendData(sendN*sendM);

		for (int j = 0; j < sendM; j++)
		{
			for (int i = 0; i < sendN; i++)
			{
				sendData[i + j*sendN] = u(i+startI, j+startJ);
			}			
		}
		
		MPI_Send(&sendData[0], sendN*sendM, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	return Field<double>(0,0);
}

void PoissonSolver::saveData(std::string outputFileName)
{
	Field<double> result = getResult();

	if(!result.isVoid())
	{
		int nn = result.getSizeI();
		int mm = result.getSizeJ();

		std::ofstream writeToFile(outputFileName);
		writeToFile << h << std::endl;
		writeToFile << nn << std::endl;

		for (int j = 0; j < mm; j++)
		{
			for (int i = 0; i < nn; i++)
			{
				writeToFile << result(i, j) << std::endl;
			}		
		}
			
		writeToFile.close();

		std::cout << "byl uložen soubor " << outputFileName << std::endl;
	}
}

void PoissonSolver::saveLocal(std::string outputFileName)
{
    std::ofstream writeToFile(outputFileName + std::to_string(rank) + ".txt");
    writeToFile << h << std::endl;
    writeToFile << n << std::endl;

    for (int j = 0; j < m; j++)
    {
        for (int i = 0; i < n; i++)
		{
			writeToFile << u(i, j) << std::endl;
		}		
    }
        
    writeToFile.close();
}