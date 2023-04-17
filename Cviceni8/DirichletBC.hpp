#ifndef DIRICHLETBC_H
#define DIRICHLETBC_H

class DirichletBC : public BoundaryCondition
{
    public:
        DirichletBC() : BoundaryCondition(NON, PHYSICAL), value(0.0) {};
        DirichletBC(Position pos_) : BoundaryCondition(pos_, PHYSICAL), value(0.0) {};
        DirichletBC(double val, Position pos_) : BoundaryCondition(pos_, PHYSICAL), value(val) {};

        double getValue();
        void setValue(double val);

        double calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;

    private:
        double value;
};

#endif // DIRICHLETBC_H