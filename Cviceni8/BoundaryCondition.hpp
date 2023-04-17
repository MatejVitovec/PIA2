#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include "Field.hpp"

class BoundaryCondition
{
    public:
        enum Position{TOP, BOTTOM, LEFT, RIGHT, NON};

        BoundaryCondition(Position pos_) : pos(pos_) {};
        
        //i j k clokwise
        virtual double calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const = 0;
        virtual void apply(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const;
        virtual void applyVirtual(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const;

    protected:
        Position pos;
};

#endif // BOUNDARYCONDITION_H