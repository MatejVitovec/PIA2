#ifndef DIRICHLETBC_H
#define DIRICHLETBC_H

class DirichletBC : public BoundaryCondition
{
    public:
        DirichletBC() : BoundaryCondition(NON), value(0.0) {};
        DirichletBC(Position pos_) : BoundaryCondition(pos_), value(0.0) {};
        DirichletBC(double val, Position pos_) : BoundaryCondition(pos_), value(val) {};

        double getValue();
        void setValue(double val);

        virtual double calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;

    private:
        double value;
};

#endif // DIRICHLETBC_H