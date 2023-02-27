#ifndef FIELD_H
#define FIELD_H

#include <vector>

template <typename T>
class Field
{
    public:
        Field();
        Field(int xMax, int yMax);

        int getSizeI() const;
        int getSizeJ() const;

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
Field<T>::Field()
{
    
}

template <typename T>
Field<T>::Field(int xMax, int yMax)
{
    iSize = xMax;
    jSize = yMax;
    data = std::vector<T>(iSize*jSize); 
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


/*template <typename T>
T Field<T>::prvni()
{
    return a;
}*/

#endif // FIELD_H