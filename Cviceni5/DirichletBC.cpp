#include <cmath>

#include "BoundaryCondition.hpp"
#include "DirichletBC.hpp"

double DirichletBC::apply() const
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