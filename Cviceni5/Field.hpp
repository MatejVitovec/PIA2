#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <omp.h>

template <typename T>
class Field
{
    public:
        Field(int n, int m) : iSize(n), jSize(m), data(n*m) {};
        Field(int n, int m, T value) : iSize(n), jSize(m), data(n*m, value) {};

        int size() const;
        int getSizeI() const;
        int getSizeJ() const;

        T normEuclid() const;

        T mean() const;

        T operator()(int i, int j) const
        {
            return data[j*iSize + i];
        }

        T& operator()(int i, int j)
        {
            return data[j*iSize + i];
        }

    private:
        std::vector<T> data;
        int iSize;
        int jSize;
        
};


template <typename T>
int Field<T>::size() const
{
    return data.size();
}

template <typename T>
int Field<T>::getSizeI() const
{
    return iSize;
}

template <typename T>
int Field<T>::getSizeJ() const
{
    return jSize;
}

template <typename T>
T Field<T>::mean() const
{
    T sum;
    int n = iSize*jSize;

    #pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < iSize*jSize; i++)
    {
        sum += data[i];
    }

    return sum/n;
}

template <typename T>
T Field<T>::normEuclid() const
{
    T aux = 0.0;

    #pragma omp parallel for reduction(+ : aux)
    for (int i = 0; i < iSize*jSize; i++)
    {
        aux += data[i]*data[i];
    }

    return sqrt(aux);
}

template <typename T>
Field<T> operator+ (const Field<T>& u, const Field<T>& v)
{
    if(u.getSizeI() == v.getSizeI() && u.getSizeJ() == v.getSizeJ())
    {
        Field<T> out(u.getSizeI(), u.getSizeJ());

        #pragma omp parallel for
        for (int j = 0; j < u.getSizeJ(); j++)
        {
            for (int i = 0; i < u.getSizeI(); i++)
            {
                out(i,j) = u(i,j) + v(i,j);
            }
        }

        return out;
    }

    return u;
}

template <typename T>
Field<T> operator-(const Field<T>& u, const Field<T>& v)
{
    if(u.getSizeI() == v.getSizeI() && u.getSizeJ() == v.getSizeJ())
    {
        Field<T> out(u.getSizeI(), u.getSizeJ());

        #pragma omp parallel for
        for (int j = 0; j < u.getSizeJ(); j++)
        {
            for (int i = 0; i < u.getSizeI(); i++)
            {
                out(i,j) = u(i,j) - v(i,j);
            }
        }

        return out;
    }

    return u;
}

template <typename T>
Field<T> operator+ (const Field<T>& u, const T& a)
{
    Field<T> out(u.getSizeI(), u.getSizeJ());

    #pragma omp parallel for
    for (int j = 0; j < u.getSizeJ(); j++)
    {
        for (int i = 0; i < u.getSizeI(); i++)
        {
            out(i,j) = u(i,j) + a;
        }
    }

    return out;
}

template <typename T>
Field<T> operator- (const Field<T>& u, const T& a)
{
    Field<T> out(u.getSizeI(), u.getSizeJ());

    #pragma omp parallel for
    for (int j = 0; j < u.getSizeJ(); j++)
    {
        for (int i = 0; i < u.getSizeI(); i++)
        {
            out(i,j) = u(i,j) - a;
        }
    }

    return out;
}

template <typename T>
Field<T> operator* (const Field<T>& u, const T& a)
{
    Field<T> out(u.getSizeI(), u.getSizeJ());

    #pragma omp parallel for
    for (int j = 0; j < u.getSizeJ(); j++)
    {
        for (int i = 0; i < u.getSizeI(); i++)
        {
            out(i,j) = u(i,j)*a;
        }
    }

    return out;
}

template <typename T>
Field<T> operator/ (const Field<T>& u, const T& a)
{
    Field<T> out(u.getSizeI(), u.getSizeJ());

    #pragma omp parallel for
    for (int j = 0; j < u.getSizeJ(); j++)
    {
        for (int i = 0; i < u.getSizeI(); i++)
        {
            out(i,j) = u(i,j)/a;
        }
    }

    return out;
}

#endif // FIELD_H