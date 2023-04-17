#include <cmath>
#include <iostream>
#include <vector>
#include <mpi.h>

#include "BoundaryCondition.hpp"
#include "VirtualBC.hpp"

double VirtualBC::calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const
{
    //nepotrebuji
    return 0.0;
}

void VirtualBC::apply(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const
{
    int n = u.getSizeI();
    int m = u.getSizeJ();

    if(pos == BOTTOM)
    {
        std::vector<double> sendData(n);
        std::vector<double> recieveData(n);

        for (int i = 0; i < n; i++)
	    {
		    sendData[i] = u(i, 1);
	    }

        MPI_Recv(&recieveData[0], n, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&sendData[0], n, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD);

        for (int i = 0; i < n; i++)
	    {
		    un(i,0) = recieveData[i];
	    }
    }
    else if(pos == TOP)
    {
        std::vector<double> sendData(n);
        std::vector<double> recieveData(n);

        for (int i = 0; i < n; i++)
	    {
		    sendData[i] = u(i, m-2);
	    }

        MPI_Send(&sendData[0], n, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD);
        MPI_Recv(&recieveData[0], n, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < n; i++)
	    {
		    un(i,m-1) = recieveData[i];
	    }
    }
    else if(pos == LEFT)
    {
        std::vector<double> sendData(m);
        std::vector<double> recieveData(m);

        for (int j = 0; j < m; j++)
	    {
		    sendData[j] = u(1, j);
	    }

        MPI_Recv(&recieveData[0], m, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&sendData[0], m, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD);

        for (int j = 0; j < m; j++)
	    {
		    un(0,j) = recieveData[j];
	    }
    }
    else if(pos == RIGHT)
    {
        std::vector<double> sendData(m);
        std::vector<double> recieveData(m);

        for (int j = 0; j < m; j++)
	    {
		    sendData[j] = u(n-2, j);
	    }

        MPI_Send(&sendData[0], m, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD);
        MPI_Recv(&recieveData[0], m, MPI_DOUBLE, neighbourRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int j = 0; j < m; j++)
	    {
		    un(n-1, j) = recieveData[j];
	    }
    }
    else
    {
        std::cout << "chyba" << std::endl;
    }
}