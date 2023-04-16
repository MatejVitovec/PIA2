#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H

#include <mpi.h>
#include <memory>
#include "Field.hpp"
#include "BoundaryCondition.hpp"

class PoissonSolver
{
    public:
        PoissonSolver();
        PoissonSolver(int rank_, int size_, double h_);

        void setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right);
        void setTargetError(double err);
        void setFunctionValues(double (*function_ptr)(int, int, double));

        void solve();

        void test();
        
        void saveData(std::string outputFileName);
        void saveLocal(std::string outputFileName);
        Field<double> getResult();

    private:
        void solveBoundaryCondition(Field<double>& un);
        //double calculateError(const Field<double>& un);

        int colsOfOptimalStructure(int size_);
        
        std::shared_ptr<BoundaryCondition> topBC;
        std::shared_ptr<BoundaryCondition> bottomBC;
        std::shared_ptr<BoundaryCondition> leftBC;
        std::shared_ptr<BoundaryCondition> rightBC;

        double h;
        int n; //radky x
        int m; //sloupce y

        //MPI
        int rank;
        int size;
        int mpiIndexX;
        int mpiIndexY;
        int mpiN;
        int mpiM;

        double targetError;

        Field<double> u;
        Field<double> func;
};

#endif // POISSONSOLVER_H