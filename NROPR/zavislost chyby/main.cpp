#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

double analytical(double x)
{
    double c = 1.0;
    return c*std::exp(-2.0*x);
}

double F(double y)
{
    return -2.0*y;
}

double initCondition()
{
    return 1.0;
}

double explicitEuler(double h, double start, double end)
{
    long n = (end - start)/h;

    double y = initCondition();

    for (long i = 0; i < n; i++)
    {
        y += h*F(y);
    }
    
    return y;
}

int main()
{
    std::ofstream writeToFile("results.txt");

    for (int i = 1; i < 42; i++)
    {
        double h = (2.0/(pow(2.0,i)));
        double result = explicitEuler(h, 0.0, 1.0);

        double E = std::fabs(analytical(1.0) - result);

        writeToFile << h << "," << E << std::endl;

        std::cout << i <<". iterace" << std::endl;
    }

    return 0;
}