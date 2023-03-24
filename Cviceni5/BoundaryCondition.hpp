#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

class BoundaryCondition
{
    public:
        //i j k clokwise
        virtual double apply(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const = 0;

    protected:

};

#endif // BOUNDARYCONDITION_H