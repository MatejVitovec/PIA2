#include <cmath>

#include "BoundaryCondition.hpp"
#include "DirichletBC.hpp"

double DirichletBC::calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const
{
    return value;
}

double DirichletBC::getValue()
{
    return value;
}

void DirichletBC::setValue(double val)
{
    value = val;
}