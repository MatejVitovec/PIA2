#include <iostream>
#include "BoundaryCondition.hpp"

void BoundaryCondition::apply(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const
{
    int nMinusOne = u.getSizeI() - 1;
	int nMinusTwo = nMinusOne - 1;
	int mMinusOne = u.getSizeJ() - 1;
	int mMinusTwo = mMinusOne - 1;
	
    if(pos == BOTTOM)
    {
        for (int i = 1; i < nMinusOne; i++)
	    {
		    un(i, 0) = calculate(u(i-1, 0), u(i, 1), u(i+1, 0), func(i, 0), h);
	    }
    }
    else if(pos == TOP)
    {
        for (int i = 1; i < nMinusOne; i++)
        {
            un(i, mMinusOne) = calculate(u(i+1, nMinusOne), u(i, nMinusTwo), u(i-1, nMinusOne), func(i, nMinusOne), h);
        }
    }
    else if(pos == LEFT)
    {
        for (int j = 1; j < mMinusOne; j++)
        {
            un(0, j) = calculate(u(0, j+1), u(1, j), u(0, j-1), func(0, j), h);
        }
    }
    else if(pos == RIGHT)
    {
        for (int j = 1; j < mMinusOne; j++)
        {
            un(nMinusOne, j) = calculate(u(mMinusOne, j-1), u(mMinusTwo, j), u(mMinusOne, j+1), func(mMinusOne, j), h);
        }
    }
    else
    {
        std::cout << "chyba" << std::endl;
    }

}