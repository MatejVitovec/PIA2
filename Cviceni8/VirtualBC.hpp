#ifndef VIRTUALbc_H
#define VIRTUALbc_H

class VirtualBC : public BoundaryCondition
{
    public:
        VirtualBC() : BoundaryCondition(NON) {};
        VirtualBC(Position pos_) : BoundaryCondition(pos_) {};
        VirtualBC(double rank_, double neighbourRank_, Position pos_) : BoundaryCondition(pos_), rank(rank_), neighbourRank(neighbourRank_) {};

        virtual double calculate(const double& u1 ,const double& u2, const double& u3, const double& f, const double& h) const;
        void apply(Field<double>& un, const Field<double>& u, const Field<double>& func, const double& h) const;

    private:
        int rank;
        int neighbourRank;
};

#endif // VIRTUALbc_H