#ifndef FIELD_H
#define FIELD_H

#include <vector>

template <typename T>
class Field
{
    public:
        Field();
        Field(int xMax, int yMax);

        T operator()(int i, int j) const
        {
            return data[j*xSize + i];
        }

        T& operator()(int i, int j)
        {
            return data[j*xSize + i];
        }

    private:
        std::vector<T> data;
        int xSize;
        int ySize;
};

template <typename T>
Field<T>::Field()
{
    
}

template <typename T>
Field<T>::Field(int xMax, int yMax)
{
    xSize = xMax + 1;
    ySize = yMax + 1;
    data = std::vector<T>(xSize*ySize); 
}



/*template <typename T>
T Field<T>::prvni()
{
    return a;
}*/

#endif // FIELD_H