#include <cmath>

#include "BoundaryCondition.hpp"
#include "NeumannBC.hpp"

double NeumannBC::calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const
{
    return 0.5*u2 + 0.25*u1 + 0.25*u3 + 0.25*h*h*f + 0.5*h*value;
}

double NeumannBC::getValue()
{
    return value;
}

void NeumannBC::setValue(double val)
{
    value = val;
}