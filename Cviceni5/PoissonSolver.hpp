#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H

#include <memory>
#include "Field.hpp"
#include "BoundaryCondition.hpp"

class PoissonSolver
{
    public:
        PoissonSolver(double h) : n((int) 1.0/h),
                                interval(1.0),
                                h(h),
                                u(n, n),
                                func(n, n),                                
                                targetError(10e-6) {};

        void setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right);
        void setTargetError(double err);
        void setInterval(double interval_);
        void setStep(double hh);
        void setFunctionValues(Field<double> f_);
        void setFunctionValues(double (*function_ptr)(int, int, double));

        void solve();
        void solveGaussSeidel();
        void saveData(std::string outputFileName);

        Field<double> getResult();

    private:
        void solveBoundaryCondition(Field<double>& un);
        void solveBoundaryConditionRedBlack(const Field<double>& u, Field<double>& un, int redBlack);
        Field<double> setRedBlack(Field<double>& u, int redBlack);
        double calculateError(const Field<double>& un);
        
        std::shared_ptr<BoundaryCondition> topBC;
        std::shared_ptr<BoundaryCondition> bottomBC;
        std::shared_ptr<BoundaryCondition> leftBC;
        std::shared_ptr<BoundaryCondition> rightBC;

        double interval;
        double h;
        int n;

        double targetError;

        Field<double> u;
        Field<double> func;
};

#endif // POISSONSOLVER_H