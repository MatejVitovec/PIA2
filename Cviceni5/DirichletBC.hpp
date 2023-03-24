#ifndef DIRICHLETBC_H
#define DIRICHLETBC_H

class DirichletBC : public BoundaryCondition
{
    public:
        DirichletBC() : value(0.0) {};
        DirichletBC(double val) : value(val) {};

        double getValue();
        void setValue(double val);

        virtual double apply(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;

    private:
        double value;
};

#endif // DIRICHLETBC_H