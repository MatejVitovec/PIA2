#ifndef MPIBC_H
#define MPIBC_H

class MPIBC : public BoundaryCondition
{
    public:
        MPIBC() : value(0.0) {};
        MPIBC(double neighbourRank_) : neighbourRank(neighbourRank_) {};


        virtual double apply(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;

    private:
        double neighbourRank;
};

#endif // MPIBC_H