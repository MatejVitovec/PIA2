#ifndef NEUMANNBC_H
#define NEUMANNBC_H

class NeumannBC : public BoundaryCondition
{
    public:
        NeumannBC() : BoundaryCondition(NON), value(0.0) {};
        NeumannBC(Position pos_) : BoundaryCondition(pos_), value(0.0) {};
        NeumannBC(double val, Position pos_) : BoundaryCondition(pos_), value(val) {};

        double getValue();
        void setValue(double val);

        double calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;

    private:
        double value;
};

#endif // NEUMMANBC_H