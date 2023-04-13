#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H

#include <memory>
#include "Field.hpp"
#include "BoundaryCondition.hpp"

class PoissonSolver
{
    public:
        PoissonSolver();
        PoissonSolver(double h_);
        /*PoissonSolver(double h, double intX, double intY);*/
        /*oissonSolver(double h) : h(h),
                                firstX(1.0),
                                firstY(1.0),
                                n((int)(1.0/h)),
                                m((int)(1.0/h)),
                                u(n, n),
                                func(n, n),                                
                                targetError(10e-6) {};*/
/*	h = h;
	firstX = 0;
	firstY = 0;
	n = (int)(1.0/h);
	m = (int)(1.0/h);
	u = Field<double>(n, m);
	func = Field<double>(n, m);
	targetError = 10e-6;*/


        void setBoundaryCondition(std::shared_ptr<BoundaryCondition>top, std::shared_ptr<BoundaryCondition>bottom, std::shared_ptr<BoundaryCondition> left, std::shared_ptr<BoundaryCondition> right);
        void setTargetError(double err);
        void setFunctionValues(Field<double> f_);
        void setFunctionValues(double (*function_ptr)(int, int, double));

        void solve();
        
        void saveData(std::string outputFileName);
        Field<double> getResult();

    private:
        void solveBoundaryCondition(Field<double>& un);
        double calculateError(const Field<double>& un);
        
        std::shared_ptr<BoundaryCondition> topBC;
        std::shared_ptr<BoundaryCondition> bottomBC;
        std::shared_ptr<BoundaryCondition> leftBC;
        std::shared_ptr<BoundaryCondition> rightBC;

        double h;
        int firstX;
        int firstY;
        int n; //radky x
        int m; //sloupce y

        double targetError;

        Field<double> u;
        Field<double> func;
};

#endif // POISSONSOLVER_H