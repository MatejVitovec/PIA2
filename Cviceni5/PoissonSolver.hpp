#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H

#include <memory>
#include "Field.h"
#include "BoundaryCondition.hpp"

class PoissonSolver
{
    public:
        PoissonSolver(int n, double h) : u(n, n), un(n, n), h(h), n(n) {};

        void setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right);
        void solve();
        void saveData(std::string outputFileName);

        Field<double> getU();

    private:
        double f(int i, int j, double h);
        void applyBoundaryCondition();
        
        std::shared_ptr<BoundaryCondition> topBC;
        std::shared_ptr<BoundaryCondition> bottomBC;
        std::shared_ptr<BoundaryCondition> leftBC;
        std::shared_ptr<BoundaryCondition> rightBC;

        const double h;
        const double n;

        Field<double> u;
        Field<double> un;

};

#endif // POISSONSOLVER_H