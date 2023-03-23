#include <cmath>

#include "BoundaryCondition.hpp"
#include "NeumannBC.hpp"

double NeumannBC::apply() const
{
    //TODO
    return value;
}

double NeumannBC::apply(int i) const
{
    //TODO
    return value;
}

double NeumannBC::getValue()
{
    return value;
}

void NeumannBC::setValue(double val)
{
    value = val;
}