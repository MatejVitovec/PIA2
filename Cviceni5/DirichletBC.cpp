#include <cmath>

#include "BoundaryCondition.hpp"
#include "DirichletBC.hpp"

double DirichletBC::apply() const
{
    return value;
}