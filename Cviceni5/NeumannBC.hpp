#ifndef NEUMANNBC_H
#define NEUMANNBC_H

class NeumannBC : public BoundaryCondition
{
    public:
        NeumannBC() : value(0.0) {};
        NeumannBC(double val) : value(val) {};

        double getValue();
        void setValue(double val);

        virtual double apply() const;
        virtual double apply(int i) const;

    private:
        double value;
};

#endif // NEUMMANBC_H