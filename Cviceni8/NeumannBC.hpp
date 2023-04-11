#ifndef NEUMANNBC_H
#define NEUMANNBC_H

class NeumannBC : public BoundaryCondition
{
    public:
        NeumannBC() : value(0.0) {};
        NeumannBC(double val) : value(val) {};

        double getValue();
        void setValue(double val);

        virtual double apply(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;

    private:
        double value;
};

#endif // NEUMMANBC_H