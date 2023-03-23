#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

class BoundaryCondition
{
    public:
        virtual double apply() const = 0;
        virtual double apply(int i) const = 0;

    protected:

};

#endif // BOUNDARYCONDITION_H