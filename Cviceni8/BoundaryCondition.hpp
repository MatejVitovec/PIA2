#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include "Field.hpp"

class BoundaryCondition
{
    public:
        enum Position{TOP, BOTTOM, LEFT, RIGHT, NON};
        enum BCType{PHYSICAL, VIRTUAL};

        BoundaryCondition(Position pos_, BCType bcType_) : pos(pos_), bcType(bcType_) {};
        
        //i j k clokwise
        virtual double calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const = 0;
        virtual void apply(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const;
        void applyPhysical(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const;
        void applyVirtual(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const;
        
    protected:
        Position pos;
        BCType bcType;
};

#endif // BOUNDARYCONDITION_H